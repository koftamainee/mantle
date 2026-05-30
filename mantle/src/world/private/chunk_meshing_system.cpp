#include "world/chunk_meshing_system.h"
#include "world/chunk_storage_system.h"

#include <glm/vec4.hpp>

#include "core/memory/pmr/arena_resource.h"
#include "core/memory/scope_arena.h"
#include "spdlog/spdlog.h"

#include "renderer/gpu_resource_manager.h"
#include "renderer/renderer.h"
#include "renderer/utils.h"

namespace mantle {

    ChunkMeshingSystem::~ChunkMeshingSystem() { destroy(); }

    void ChunkMeshingSystem::init(Renderer &renderer,
                                  ArenaAllocator &scratch_arena,
                                  u32 max_chunks) {
        check(!m_is_initialized);

        m_renderer = &renderer;
        m_max_chunks = max_chunks;
        auto &rm = renderer.resource_manager();

        m_chunk_data = rm.create_buffer(
            {
                .size = max_chunks * sizeof(Chunk),
                .usage = BufferUsage::Storage | BufferUsage::Transfer,
                .memory = MemoryType::CpuToGpu,
            },
            true);

        m_chunk_metadata = rm.create_buffer(
            {
                .size = max_chunks * sizeof(glm::ivec3),
                .usage = BufferUsage::Storage | BufferUsage::Transfer,
                .memory = MemoryType::CpuToGpu,
            },
            true);

        u32 faces_per_chunk = Chunk::volume * 6;
        m_max_faces = max_chunks * faces_per_chunk;

        u64 vertex_buffer_size =
            static_cast<u64>(m_max_faces) * sizeof(glm::vec4) * 4;
        m_vertex_buffer = rm.create_buffer({
            .size = vertex_buffer_size,
            .usage = BufferUsage::Storage | BufferUsage::Vertex |
                BufferUsage::Transfer,
        });

        u64 index_buffer_size =
            static_cast<u64>(m_max_faces) * sizeof(u32) * 6;
        m_index_buffer = rm.create_buffer({
            .size = index_buffer_size,
            .usage = BufferUsage::Storage | BufferUsage::Index |
                BufferUsage::Transfer,
        });

        spdlog::info("Chunk meshing buffers: {} faces max ({} MB vertex, {} MB index)",
                      m_max_faces,
                      vertex_buffer_size / (1024 * 1024),
                      index_buffer_size / (1024 * 1024));

        m_indirect_buffer = rm.create_buffer({
            .size = 1024,
            .usage = BufferUsage::Storage | BufferUsage::Indirect |
                BufferUsage::Transfer,
        });

        m_counter_buffer = rm.create_buffer({
            .size = sizeof(u32),
            .usage = BufferUsage::Storage | BufferUsage::Transfer,
            .memory = MemoryType::CpuToGpu,
        }, true);

        u32 zero = 0;
        rm.update_buffer(m_counter_buffer, &zero, sizeof(u32));

        m_dispatch_cmd_buffer = rm.create_buffer(
            {
                .size = sizeof(DispatchCmd),
                .usage = BufferUsage::Indirect | BufferUsage::Transfer,
                .memory = MemoryType::CpuToGpu,
            },
            true);

        ScopeArena scope(&scratch_arena);
        ArenaResource resource(&scratch_arena);
        std::pmr::vector<u32> spv(&resource);

        load_spv("assets/shaders/mesh_chunks.spv", spv);
        ShaderHandle meshing_shader = rm.create_shader(spv);
        m_meshing_pipeline = rm.create_compute_pipeline({
            .shader =
                {
                    .entry_point = "main",
                    .stage = ShaderStage::Compute,
                    .shader = meshing_shader,
                },
            .push_constants =
                {
                    .stage = ShaderStage::Compute,
                    .size = sizeof(MeshingPC),
                    .offset = 0,
                },
        });
        rm.destroy_shader(meshing_shader, true);

        spv.clear();
        load_spv("assets/shaders/write_indirect.spv", spv);
        ShaderHandle indirect_shader = rm.create_shader(spv);
        m_indirect_pipeline = rm.create_compute_pipeline({
            .shader =
                {
                    .entry_point = "main",
                    .stage = ShaderStage::Compute,
                    .shader = indirect_shader,
                },
            .push_constants =
                {
                    .stage = ShaderStage::Compute,
                    .size = sizeof(WriteIndirectPC),
                    .offset = 0,
                },
        });
        rm.destroy_shader(indirect_shader, true);

        m_is_initialized = true;
    }

    void ChunkMeshingSystem::upload_dirty(Renderer &renderer,
                                          ChunkStorageSystem &storage) {
        check(m_is_initialized);

        auto &rm = renderer.resource_manager();

        // Check previous frame's face count for overflow warning
        u32 prev_face_count = 0;
        rm.read_buffer(m_counter_buffer, &prev_face_count, sizeof(u32));
        if (prev_face_count > m_max_faces) {
            spdlog::warn("Vertex buffer overflow: {} faces, max {}",
                         prev_face_count, m_max_faces);
        } else if (prev_face_count > m_max_faces * 9 / 10) {
            spdlog::warn("Vertex buffer near capacity: {}/{} faces used ({}%)",
                          prev_face_count, m_max_faces,
                          prev_face_count * 100 / m_max_faces);
        }

        for (u32 i : storage.dirty_indices()) {
            if (!storage.is_dirty(i)) {
                continue;
            }

            rm.update_buffer(m_chunk_data, &storage.get_chunk(i), sizeof(Chunk),
                             i * sizeof(Chunk));

            glm::ivec3 pos = storage.get_position(i);
            rm.update_buffer(m_chunk_metadata, &pos, sizeof(glm::ivec3),
                             i * sizeof(glm::ivec3));

            storage.mark_clean(i);
        }

        storage.clear_dirty();

        u32 chunk_count = storage.count();
        if (chunk_count != m_last_chunk_count) {
            m_last_chunk_count = chunk_count;

            // DispatchIndirectCommand for mesh_chunks.slang:
            //   {4, 4, 8 * count} workgroups × {8, 8, 4} threads
            //   = {32, 32, 32 * count} total threads
            //   = one 32×32×32 voxel slab per chunk, count chunks stacked in Z
            DispatchCmd cmd = {4, 4, 8 * chunk_count};
            rm.update_buffer(m_dispatch_cmd_buffer, &cmd, sizeof(cmd), 0);
        }
    }

    void ChunkMeshingSystem::add_passes(FrameGraph &graph,
                                        Blackboard &blackboard) const {
        check(m_is_initialized);
        check(m_max_chunks > 0);

        FGBufferHandle chunk_fg = graph.import_buffer(m_chunk_data);
        FGBufferHandle metadata_fg = graph.import_buffer(m_chunk_metadata);
        FGBufferHandle vertex_fg = graph.import_buffer(m_vertex_buffer);
        FGBufferHandle index_fg = graph.import_buffer(m_index_buffer);
        FGBufferHandle indirect_fg = graph.import_buffer(m_indirect_buffer);
        FGBufferHandle counter_fg = graph.import_buffer(m_counter_buffer);
        FGBufferHandle dispatch_cmd_fg =
            graph.import_buffer(m_dispatch_cmd_buffer);

        struct FillPass final {
            FGBufferHandle counter;
        };
        const auto &fill = graph.add_pass<FillPass>(
            "Fill Counter",
            [&](FrameGraphBuilder &builder, FillPass &pass) {
                pass.counter =
                    builder.write(counter_fg, BufferWriteUsage::TransferDst);
            },
            [](FGPassContext &ctx, const FillPass &pass) {
                ctx.fill_buffer({.dst = pass.counter, .value = 0, .size = 4});
            });

        struct MeshPass final {
            FGBufferHandle chunk;
            FGBufferHandle metadata;
            FGBufferHandle vertex;
            FGBufferHandle index;
            FGBufferHandle indirect;
            FGBufferHandle counter;
            FGBufferHandle dispatch_cmd;
        };
        const auto &mesh = graph.add_pass<MeshPass>(
            "Mesh Chunks",
            [&](FrameGraphBuilder &builder, MeshPass &pass) {
                pass.chunk = builder.read(chunk_fg, BufferReadUsage::Storage);
                pass.metadata =
                    builder.read(metadata_fg, BufferReadUsage::Storage);
                pass.counter = builder.write(
                    builder.read(fill.counter, BufferReadUsage::Storage),
                    BufferWriteUsage::Storage);
                pass.vertex =
                    builder.write(vertex_fg, BufferWriteUsage::Storage);
                pass.index = builder.write(index_fg, BufferWriteUsage::Storage);
                pass.indirect =
                    builder.write(indirect_fg, BufferWriteUsage::Storage);
                pass.dispatch_cmd =
                    builder.read(dispatch_cmd_fg, BufferReadUsage::IndirectArg);
            },
            [this](FGPassContext &ctx, const MeshPass &pass) {
                ctx.bind_pipeline(m_meshing_pipeline);
                MeshingPC pc = meshing_pc();
                ctx.push_constants(&pc, ShaderStage::Compute);
                ctx.dispatch_indirect({.buffer = pass.dispatch_cmd});
            });

        struct IndirectPass final {
            FGBufferHandle counter;
            FGBufferHandle indirect;
        };
        const auto &_ = graph.add_pass<IndirectPass>(
            "Write Indirect",
            [&](FrameGraphBuilder &builder, IndirectPass &pass) {
                pass.counter =
                    builder.read(mesh.counter, BufferReadUsage::Storage);
                pass.indirect = builder.write(
                    builder.read(mesh.indirect, BufferReadUsage::Storage),
                    BufferWriteUsage::Storage);
            },
            [this](FGPassContext &ctx, const IndirectPass &pass) {
                ctx.bind_pipeline(m_indirect_pipeline);
                WriteIndirectPC pc = indirect_pc();
                ctx.push_constants(&pc, ShaderStage::Compute);
                ctx.dispatch({1, 1, 1});
            });

        blackboard.add(BbChunkVertex{vertex_fg});
        blackboard.add(BbChunkIndex{index_fg});
        blackboard.add(BbChunkIndirect{indirect_fg});
    }

    void ChunkMeshingSystem::destroy() {
        if (m_is_initialized) {
            auto &rm = m_renderer->resource_manager();
            rm.destroy_buffer(m_chunk_data);
            rm.destroy_buffer(m_chunk_metadata);
            rm.destroy_buffer(m_vertex_buffer);
            rm.destroy_buffer(m_index_buffer);
            rm.destroy_buffer(m_indirect_buffer);
            rm.destroy_buffer(m_counter_buffer);
            rm.destroy_buffer(m_dispatch_cmd_buffer);
            rm.destroy_compute_pipeline(m_meshing_pipeline);
            rm.destroy_compute_pipeline(m_indirect_pipeline);
            m_is_initialized = false;
        }
    }

    ChunkMeshingSystem::MeshingPC ChunkMeshingSystem::meshing_pc() const {
        auto &rm = m_renderer->resource_manager();
        return {
            .chunk_buffer_index = rm.get_bindless_index(m_chunk_data),
            .metadata_buffer_index = rm.get_bindless_index(m_chunk_metadata),
            .vertex_buffer_index = rm.get_bindless_index(m_vertex_buffer),
            .index_buffer_index = rm.get_bindless_index(m_index_buffer),
            .indirect_buffer_index = rm.get_bindless_index(m_indirect_buffer),
            .counter_buffer_index = rm.get_bindless_index(m_counter_buffer),
            .max_faces = m_max_faces,
        };
    }

    ChunkMeshingSystem::WriteIndirectPC
    ChunkMeshingSystem::indirect_pc() const {
        auto &rm = m_renderer->resource_manager();
        return {
            .counter_buffer_index = rm.get_bindless_index(m_counter_buffer),
            .indirect_buffer_index = rm.get_bindless_index(m_indirect_buffer),
            .max_faces = m_max_faces,
        };
    }

} // namespace mantle

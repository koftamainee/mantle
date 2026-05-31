#include "world/chunk_meshing_system.h"
#include "world/chunk_storage_system.h"

#include <array>
#include <cstring>
#include <vector>

#include "bgm.h"
#include "core/assert.h"
#include "core/memory/pmr/arena_resource.h"
#include "core/memory/scope_arena.h"
#include "helpers.h"
#include "math/aabb.h"
#include "math/frustum.h"
#include "renderer/blackboard_types.h"
#include "renderer/gpu_resource_manager.h"
#include "renderer/renderer.h"
#include "renderer/utils.h"
#include "spdlog/spdlog.h"

namespace mantle {

    static constexpr f32 VOXEL_SCALE = 0.1f;
    static constexpr f32 CHUNK_WORLD_SIZE = (f32)Chunk::size * VOXEL_SCALE;

    static void get_neighbors(const ChunkStorageSystem &storage, glm::ivec3 pos,
                              const Chunk *out[6]) {
        static const glm::ivec3 offsets[6] = {
            {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1},
        };
        for (u32 i = 0; i < 6; i++) {
            glm::ivec3 nb_pos = pos + offsets[i];
            out[i] = storage.has_chunk(nb_pos) ? &storage.get_chunk(nb_pos)
                                               : nullptr;
        }
    }


    ChunkMeshingSystem::~ChunkMeshingSystem() { destroy(); }

    void ChunkMeshingSystem::init(Renderer &renderer,
                                  ArenaAllocator &scratch_arena,
                                  u32 max_chunks) {
        check(!m_is_initialized);

        m_renderer = &renderer;
        m_max_chunks = max_chunks;
        auto &rm = renderer.resource_manager();

        m_slots = scratch_arena.push<ChunkMeshSlot>(max_chunks);
        std::memset(m_slots, 0, sizeof(ChunkMeshSlot) * max_chunks);

        ScopeArena scope(&scratch_arena);
        ArenaResource resource(&scratch_arena);
        std::pmr::vector<u32> spv(&resource);

        load_spv("assets/shaders/mesh_render.spv", spv);
        ShaderHandle shader = rm.create_shader(spv);

        ShaderModule modules[] = {
            {"vert_main", ShaderStage::Vertex, shader},
            {"frag_main", ShaderStage::Fragment, shader},
        };
        ImageFormat color_format = renderer.get_swapchain_info().surface_format;

        ColorBlendAttachment attachment = {};
        ColorBlendState blend = {};
        blend.attachments = span(attachment);

        DepthStencilState depth_stencil = {
            .depth_test_enable = true,
            .depth_write_enable = true,
            .depth_compare_op = CompareOp::Less,
        };

        VertexBinding bindings[] = {{0, 16}};
        VertexAttribute attributes[] = {
            {0, 0, VertexFormat::Float3, 0},
            {1, 0, VertexFormat::Uint1, 12},
        };

        PushConstantsRange pc[] = {
            {ShaderStage::Vertex, sizeof(glm::mat4), 0},
        };

        m_mesh_pipeline = rm.create_graphics_pipeline({
            .shaders = modules,
            .vertex_input = {bindings, attributes},
            .rasterization = {.cull_mode = CullMode::Back,
                              .front_face = FrontFace::CounterClockwise},
            .depth_stencil = depth_stencil,
            .color_blend = blend,
            .color_formats = span(color_format),
            .depth_format = ImageFormat::D32,
            .push_constants = pc,
        });
        rm.destroy_shader(shader, true);

        m_vertex_stride = MAX_QUADS_PER_CHUNK * 4 * sizeof(MeshVertex);
        m_index_stride = MAX_QUADS_PER_CHUNK * 6 * sizeof(u32);

        m_vertex_buffer = rm.create_buffer(
            {
                .size = static_cast<u64>(max_chunks) * m_vertex_stride,
                .usage = BufferUsage::Vertex | BufferUsage::Transfer,
                .memory = MemoryType::CpuToGpu,
            },
            true);

        m_index_buffer = rm.create_buffer(
            {
                .size = static_cast<u64>(max_chunks) * m_index_stride,
                .usage = BufferUsage::Index | BufferUsage::Transfer,
                .memory = MemoryType::CpuToGpu,
            },
            true);

        for (u32 i = 0; i < max_chunks; i++) {
            m_slots[i].vertex_offset = i * m_vertex_stride;
            m_slots[i].index_offset = i * m_index_stride;
        }

        m_is_initialized = true;
        spdlog::info(
            "Chunk meshing system initialized ({} chunks, {} MB total)",
            max_chunks,
            (static_cast<u64>(max_chunks) * (m_vertex_stride + m_index_stride)) /
                (1024 * 1024));
    }

    void ChunkMeshingSystem::destroy() {
        if (m_is_initialized) {
            auto &rm = m_renderer->resource_manager();
            rm.destroy_buffer(m_vertex_buffer);
            rm.destroy_buffer(m_index_buffer);
            rm.destroy_graphics_pipeline(m_mesh_pipeline);
            m_is_initialized = false;
            spdlog::info("Chunk meshing system destroyed");
        }
    }

    namespace {

        struct MeshTaskData {
            const ChunkStorageSystem *storage;
            u32 idx;
            MeshVertex *vertex_out;
            u32 *index_out;
            u32 max_quads;
            u32 quad_count;
        };

        void mesh_work(ArenaAllocator &scratch, void *data) {
            auto *d = static_cast<MeshTaskData *>(data);
            const Chunk &chunk = d->storage->get_chunk(d->idx);
            glm::ivec3 pos = d->storage->get_position(d->idx);

            const Chunk *neighbors[6];
            get_neighbors(*d->storage, pos, neighbors);

            ChunkMeshData mesh =
                bgm::mesh_chunk(chunk, neighbors, pos, scratch);

            u32 q = mesh.quad_count;
            if (q > d->max_quads) {
                q = d->max_quads;
            }
            d->quad_count = q;

            if (q > 0) {
                std::memcpy(d->vertex_out, mesh.vertices, mesh.vertex_bytes());
                std::memcpy(d->index_out, mesh.indices, mesh.index_bytes());
            }

            scratch.reset();
        }

    } // namespace

    void ChunkMeshingSystem::upload_dirty(Renderer &renderer,
                                          ChunkStorageSystem &storage,
                                          ArenaAllocator &scratch,
                                          WorkerPool *pool) const {
        check(m_is_initialized);

        if (!storage.any_dirty())
            return;

        auto &rm = renderer.resource_manager();
        const auto &dirty = storage.dirty_indices();

        if (pool) {
            std::vector<MeshTaskData> tasks;
            tasks.reserve(dirty.size());

            u32 count = 0;
            for (u32 i : dirty) {
                if (storage.is_dirty(i))
                    count++;
            }

            usize vert_stride = MAX_QUADS_PER_CHUNK * 4 * sizeof(MeshVertex);
            usize idx_stride = MAX_QUADS_PER_CHUNK * 6 * sizeof(u32);
            void *vert_block = scratch.push(vert_stride * count, 16);
            void *idx_block = scratch.push(idx_stride * count, 16);

            u32 slot = 0;
            for (u32 i : dirty) {
                if (!storage.is_dirty(i))
                    continue;
                tasks.push_back({
                    .storage = &storage,
                    .idx = i,
                    .vertex_out = static_cast<MeshVertex *>(vert_block) +
                        slot * MAX_QUADS_PER_CHUNK * 4,
                    .index_out =
                        static_cast<u32 *>(idx_block) + slot * MAX_QUADS_PER_CHUNK * 6,
                    .max_quads = MAX_QUADS_PER_CHUNK,
                    .quad_count = 0,
                });
                pool->submit(mesh_work, &tasks.back());
                slot++;
            }
            pool->wait();

            for (auto &t : tasks) {
                u32 q = t.quad_count;
                glm::ivec3 pos = t.storage->get_position(t.idx);
                auto &mesh_slot = m_slots[t.idx];
                mesh_slot.position_x = pos.x;
                mesh_slot.position_y = pos.y;
                mesh_slot.position_z = pos.z;
                mesh_slot.quad_count = q;

                if (q > 0) {
                    rm.update_buffer(m_vertex_buffer, t.vertex_out,
                                     static_cast<usize>(q) * 4 * sizeof(MeshVertex),
                                     mesh_slot.vertex_offset);
                    rm.update_buffer(m_index_buffer, t.index_out,
                                     static_cast<usize>(q) * 6 * sizeof(u32),
                                     mesh_slot.index_offset);
                }

                storage.mark_clean(t.idx);
            }
        } else {
            for (u32 i : dirty) {
                ScopeArena scope(&scratch);
                if (!storage.is_dirty(i)) {
                    continue;
                }

                const Chunk &chunk = storage.get_chunk(i);
                glm::ivec3 pos = storage.get_position(i);

                const Chunk *neighbors[6];
                get_neighbors(storage, pos, neighbors);

                ChunkMeshData mesh =
                    bgm::mesh_chunk(chunk, neighbors, pos, scratch);

                u32 q = mesh.quad_count;
                if (q > MAX_QUADS_PER_CHUNK) {
                    spdlog::warn("Chunk mesh truncated: {} → {} quads", q,
                                 MAX_QUADS_PER_CHUNK);
                    q = MAX_QUADS_PER_CHUNK;
                }

                auto &slot = m_slots[i];
                slot.position_x = pos.x;
                slot.position_y = pos.y;
                slot.position_z = pos.z;
                slot.quad_count = q;

                if (q > 0) {
                    rm.update_buffer(m_vertex_buffer, mesh.vertices,
                                     (usize)q * 4 * sizeof(MeshVertex),
                                     slot.vertex_offset);
                    rm.update_buffer(m_index_buffer, mesh.indices,
                                     (usize)q * 6 * sizeof(u32),
                                     slot.index_offset);
                }

                storage.mark_clean(i);
            }
        }

        storage.clear_dirty();
    }


    void
    ChunkMeshingSystem::add_passes(FrameGraph &graph,
                                        Blackboard &blackboard) const {
        check(m_is_initialized);

        auto backbuffer = blackboard.get<BbBackbuffer>().handle;
        const auto &camera_data = blackboard.get<BbCameraData>();
        const auto &fb_size = blackboard.get<BbFramebufferSize>();

        u32 width = fb_size.width;
        u32 height = fb_size.height;

        FGBufferHandle vertex_fg = graph.import_buffer(m_vertex_buffer);
        FGBufferHandle index_fg = graph.import_buffer(m_index_buffer);

        struct RenderPass {
            FGBufferHandle vertex;
            FGBufferHandle index;
            FGImageHandle color;
            FGImageHandle depth;
        };

        graph.add_pass<RenderPass>(
            "Render Chunk Meshes",
            [&](FrameGraphBuilder &builder, RenderPass &pass) {
                pass.depth = builder.create_image({
                    .width = width,
                    .height = height,
                    .format = ImageFormat::D32,
                    .usage = ImageUsage::Depth,
                });
                pass.vertex = builder.read(vertex_fg, BufferReadUsage::Vertex);
                pass.index = builder.read(index_fg, BufferReadUsage::Index);
                pass.color =
                    builder.write(backbuffer, WriteUsage::ColorAttachment);
                builder.write(pass.depth, WriteUsage::DepthAttachment);
            },
            [this, width, height, camera_data](FGPassContext &ctx,
                                               const RenderPass &pass) {
                Frustum frustum;
                frustum.extract(camera_data.view_proj);

                std::array<FGColorAttachment, 1> colors = {{
                    {
                        .image = pass.color,
                        .load = AttachmentLoad::Clear,
                        .store = AttachmentStore::Store,
                    },
                }};
                FGDepthAttachment depth_att = {
                    .image = pass.depth,
                    .load = AttachmentLoad::Clear,
                    .store = AttachmentStore::DontCare,
                    .clear_value = 1.0f,
                };
                ctx.begin_rendering({
                    .colors = colors,
                    .depth = &depth_att,
                    .width = width,
                    .height = height,
                });
                ctx.set_viewport(0, 0, static_cast<f32>(width),
                                 static_cast<f32>(height));
                ctx.set_scissor(0, 0, width, height);
                ctx.bind_pipeline(m_mesh_pipeline);
                ctx.push_constants(&camera_data.view_proj, ShaderStage::Vertex);

                for (u32 i = 0; i < m_max_chunks; i++) {
                    const auto &slot = m_slots[i];
                    if (slot.quad_count == 0)
                        continue;

                    glm::vec3 base =
                        glm::vec3(static_cast<f32>(slot.position_x), static_cast<f32>(slot.position_y),
                                  static_cast<f32>(slot.position_z)) *
                        CHUNK_WORLD_SIZE;
                    AABB aabb{base, base + CHUNK_WORLD_SIZE};
                    if (!frustum.intersects(aabb)) {
                        continue;
                    }

                    ctx.bind_vertex_buffer(pass.vertex, 0, slot.vertex_offset);
                    ctx.bind_index_buffer(pass.index, slot.index_offset);
                    ctx.draw_indexed({slot.quad_count * 6, 1, 0, 0, 0});
                }

                ctx.end_rendering();
            });
    }

} // namespace mantle

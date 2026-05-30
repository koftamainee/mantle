#pragma once
#include "chunk.h"
#include "chunk_meshing_types.h"
#include "core/macros.h"
#include "core/types.h"
#include "renderer/blackboard.h"
#include "renderer/frame_graph.h"
#include "renderer/types.h"

namespace mantle {
    class ArenaAllocator;
    class ChunkStorageSystem;
    class Renderer;

    class ChunkMeshingSystem final {
      public:
        struct DispatchCmd {
            u32 x;
            u32 y;
            u32 z;
        };

        struct MeshingPC final {
            u32 chunk_buffer_index;
            u32 metadata_buffer_index;
            u32 vertex_buffer_index;
            u32 index_buffer_index;
            u32 indirect_buffer_index;
            u32 counter_buffer_index;
            u32 max_faces;
        };

        struct WriteIndirectPC final {
            u32 counter_buffer_index;
            u32 indirect_buffer_index;
            u32 max_faces;
        };

        ChunkMeshingSystem() = default;
        ~ChunkMeshingSystem();

        MANTLE_NO_COPY_NO_MOVE(ChunkMeshingSystem);

        void init(Renderer &renderer, ArenaAllocator &scratch_arena,
                  u32 max_chunks);
        void upload_dirty(Renderer &renderer,
                          ChunkStorageSystem &storage);
        void add_passes(FrameGraph &graph, Blackboard &blackboard) const;
        void destroy();

      private:
        MeshingPC meshing_pc() const;
        WriteIndirectPC indirect_pc() const;

        bool m_is_initialized = false;
        u32 m_max_chunks = 0;
        u32 m_max_faces = 0;
        u32 m_last_chunk_count = 0;

        Renderer *m_renderer = nullptr;

        BufferHandle m_chunk_data{};
        BufferHandle m_chunk_metadata{};
        BufferHandle m_vertex_buffer{};
        BufferHandle m_index_buffer{};
        BufferHandle m_indirect_buffer{};
        BufferHandle m_counter_buffer{};
        BufferHandle m_dispatch_cmd_buffer{};

        ComputePipelineHandle m_meshing_pipeline{};
        ComputePipelineHandle m_indirect_pipeline{};
    };
} // namespace mantle

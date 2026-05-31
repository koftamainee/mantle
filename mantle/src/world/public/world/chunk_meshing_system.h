#pragma once
#include "core/concurrency/worker_pool.h"
#include "core/macros.h"
#include "core/types.h"
#include "glm/vec3.hpp"
#include "renderer/blackboard.h"
#include "renderer/frame_graph.h"
#include "renderer/types.h"
#include "world/chunk.h"

namespace mantle {
    class ArenaAllocator;
    class ChunkStorageSystem;
    class Renderer;

    class ChunkMeshingSystem final {
      public:
        static constexpr u32 MAX_QUADS_PER_CHUNK = 8192;

        struct ChunkMeshSlot {
            i32 position_x, position_y, position_z;
            u32 vertex_offset, index_offset, quad_count;
        };

        ChunkMeshingSystem() = default;
        ~ChunkMeshingSystem();

        MANTLE_NO_COPY_NO_MOVE(ChunkMeshingSystem);

        void init(Renderer &renderer, ArenaAllocator &scratch_arena,
                  u32 max_chunks);
        void upload_dirty(Renderer &renderer, ChunkStorageSystem &storage,
                          ArenaAllocator &scratch, WorkerPool *pool = nullptr) const;
        void add_passes(FrameGraph &graph, Blackboard &blackboard) const;
        void destroy();

      private:
        bool m_is_initialized = false;
        u32 m_max_chunks = 0;
        Renderer *m_renderer = nullptr;
        GraphicsPipelineHandle m_mesh_pipeline{};

        BufferHandle m_vertex_buffer{};
        BufferHandle m_index_buffer{};
        u32 m_vertex_stride = 0;
        u32 m_index_stride = 0;

        ChunkMeshSlot *m_slots = nullptr;
    };
} // namespace mantle

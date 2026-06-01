#pragma once
#include "core/concurrency/worker_pool.h"
#include "core/macros.h"
#include "core/types.h"
#include "glm/vec3.hpp"
#include "world/chunk.h"

namespace spdlog { class logger; }

namespace mantle {
    class ArenaAllocator;
    class ChunkRenderingSystem;
    class ChunkStorageSystem;
    class Renderer;

    struct ChunkMeshSlot {
        i32 position_x, position_y, position_z;
        u32 vertex_offset, index_offset, quad_count;
    };

    class ChunkMeshingSystem final {
      public:
        static constexpr u32 MAX_QUADS_PER_CHUNK = 8192;

        ChunkMeshingSystem() = default;
        ~ChunkMeshingSystem();

        MANTLE_NO_COPY_NO_MOVE(ChunkMeshingSystem);

        void init();
        void upload_dirty(Renderer &renderer, ChunkStorageSystem &storage,
                          ArenaAllocator &scratch, WorkerPool *pool,
                          ChunkRenderingSystem &rendering) const;
        void destroy();

      private:
        bool m_is_initialized = false;

        mutable spdlog::logger *m_logger = nullptr;
    };
} // namespace mantle

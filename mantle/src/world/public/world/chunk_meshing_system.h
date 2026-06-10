// Copyright (c) 2026 Mantle. All rights reserved.

#pragma once

#include <glm/vec3.hpp>

#include "core/concurrency/worker_pool.h"
#include "core/macros.h"
#include "core/memory/arena_allocator.h"
#include "core/memory/memory_block.h"
#include "core/types.h"
#include "world/chunk.h"

namespace spdlog {
    class logger;
}

namespace mantle {
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

        MANTLE_DEFAULT_INIT(ChunkMeshingSystem);

        void init(MemoryBlock block);
        void upload_dirty(Renderer &renderer, ChunkStorageSystem &storage,
                          WorkerPool *pool, ChunkRenderingSystem &rendering);
        void destroy();

      private:
        bool m_is_initialized = false;

        ArenaAllocator        m_scratch {};
        mutable spdlog::logger *m_logger = nullptr;
    };
} // namespace mantle

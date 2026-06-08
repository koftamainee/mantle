// Copyright (c) 2026 Mantle. All rights reserved.

#pragma once

#include <glm/vec3.hpp>

#include "core/macros.h"
#include "core/types.h"
#include "renderer/blackboard.h"
#include "renderer/frame_graph.h"
#include "renderer/types.h"
#include "world/chunk_meshing_system.h"

namespace spdlog {
    class logger;
}

namespace mantle {
    class ArenaAllocator;
    class Renderer;

    class ChunkRenderingSystem final {
      public:
        MANTLE_DEFAULT_INIT(ChunkRenderingSystem);

        void init(Renderer &renderer, ArenaAllocator &scratch_arena, u32 max_chunks);
        void add_passes(FrameGraph &graph, const Blackboard &blackboard) const;
        void destroy();

        BufferHandle   vertex_buffer() const { return m_vertex_buffer; }
        BufferHandle   index_buffer() const { return m_index_buffer; }
        ChunkMeshSlot *slots() const { return m_slots; }
        u32            max_chunks() const { return m_max_chunks; }

      private:
        bool                   m_is_initialized = false;
        u32                    m_max_chunks = 0;
        Renderer              *m_renderer = nullptr;
        GraphicsPipelineHandle m_mesh_pipeline {};

        BufferHandle m_vertex_buffer {};
        BufferHandle m_index_buffer {};
        u32          m_vertex_stride = 0;
        u32          m_index_stride = 0;

        ChunkMeshSlot *m_slots = nullptr;

        spdlog::logger *m_logger = nullptr;
    };
} // namespace mantle

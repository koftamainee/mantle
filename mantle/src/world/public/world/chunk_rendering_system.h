#pragma once
#include "core/macros.h"
#include "core/types.h"
#include "renderer/blackboard.h"
#include "renderer/frame_graph.h"
#include "renderer/types.h"

namespace mantle {
    class ArenaAllocator;
    class Renderer;

    class ChunkRenderingSystem final {
      public:
        ChunkRenderingSystem() = default;
        ~ChunkRenderingSystem();

        MANTLE_NO_COPY_NO_MOVE(ChunkRenderingSystem);

        void init(Renderer &renderer, ArenaAllocator &scratch_arena);
        void add_passes(FrameGraph &graph, Blackboard &blackboard) const;
        void destroy();

      private:
        bool m_is_initialized = false;

        Renderer *m_renderer = nullptr;

        GraphicsPipelineHandle m_mesh_pipeline{};
    };
} // namespace mantle

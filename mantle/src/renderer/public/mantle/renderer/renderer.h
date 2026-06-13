// Copyright 2026 Mantle
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "frame_graph.h"
#include "gpu_resource_manager.h"
#include "mantle/core/memory/arena_allocator.h"
#include "mantle/core/memory/memory_block.h"
#include "mantle/core/memory/tlsf_allocator.h"
#include "mantle/core/types.h"
#include "mantle/renderer/types.h"

namespace spdlog {
    class logger;
}

namespace mantle {

    class Window;

    class Renderer final {
      public:
        enum class Result {
            Ok,
            FrameNeedsResize,
        };

        MANTLE_DEFAULT_INIT(Renderer);

        void init(const Window &window, bool vsync, MemoryBlock block);
        void destroy();

        Result begin_frame();
        Result end_frame();

        GPUResourceManager &resource_manager();
        ImageHandle         backbuffer() const;

        void execute(FrameGraph &render_graph);

        ArenaAllocator &frame_arena() { return m_frame_arena; }

        void             resize_swapchain(u32 width, u32 height);
        SwapchainInfo    get_swapchain_info();
        std::string_view gpu_name() const;
        u64              vram_bytes() const;

      private:
        bool m_is_initialized = false;

        struct Impl;
        Impl *m_impl = nullptr;

        TlsfAllocator  m_perm_allocator {};
        ArenaAllocator m_frame_arena {};

        spdlog::logger *m_logger = nullptr;
    };

} // namespace mantle

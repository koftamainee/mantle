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

#include "mantle/core/memory/arena_allocator.h"
#include "mantle/core/memory/memory_block.h"
#include "mantle/core/memory/tlsf_allocator.h"
#include "mantle/core/types.h"
#include "mantle/renderer/types.h"
#include "mantle/window/window.h"
#include "vulkan_context.h"
#include "vulkan_cpu_allocator.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"

namespace spdlog {
    class logger;
}

namespace mantle {
    enum class SwapchainResult {
        Ok,
        Suboptimal,
        OutOfDate,
    };

    struct AcquiredImage final {
        SwapchainResult result;
        u32             image_index;
    };


    class VulkanBackend final {
      public:
        MANTLE_DEFAULT_INIT(VulkanBackend);

        void init(const Window &window, bool vsync, MemoryBlock block,
                  ArenaAllocator *scratch_arena);
        void destroy();
        void wait_idle() const;

        void          rebuild_swapchain(u32 width, u32 height);
        SwapchainInfo get_swapchain_info() const;

        std::string_view gpu_name() const;
        u64              vram_bytes() const;


      private:
        friend class FrameScheduler;
        friend class GPUResourceManager;

        AcquiredImage   acquire_next_image(VkSemaphore image_available) const;
        SwapchainResult present(u32 image_index, VkSemaphore render_finished) const;

        bool m_is_initialized = false;

        VulkanContext   m_context {};
        VulkanDevice    m_device {};
        VulkanSwapchain m_swapchain {};

        ArenaAllocator    *m_scratch_arena = nullptr;
        TlsfAllocator      m_tlsf_allocator {};
        VulkanCPUAllocator m_vk_allocator {};

        bool m_vsync = true;

        spdlog::logger *m_logger = nullptr;
    };


} // namespace mantle

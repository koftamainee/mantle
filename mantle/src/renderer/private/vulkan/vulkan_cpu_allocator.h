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

#include <vulkan/vulkan.h>

#include "mantle/core/macros.h"
#include "mantle/core/memory/tlsf_allocator.h"

namespace mantle {

    class VulkanCPUAllocator final {
      public:
        VulkanCPUAllocator() = default;
        ~VulkanCPUAllocator() = default;

        MANTLE_NO_COPY_NO_MOVE(VulkanCPUAllocator);

        void init(TlsfAllocator *tlsf);

        VkAllocationCallbacks *vk_allocator();

      private:
        static void *VKAPI_CALL vk_alloc(void *user, usize size, usize align,
                                         VkSystemAllocationScope scope);

        static void *VKAPI_CALL vk_realloc(void *user, void *original, usize size, usize align,
                                           VkSystemAllocationScope scope);

        static void VKAPI_CALL vk_free(void *user, void *memory);

        static void VKAPI_CALL vk_internal_alloc(void *user, usize size,
                                                 VkInternalAllocationType type,
                                                 VkSystemAllocationScope  scope);

        static void VKAPI_CALL vk_internal_free(void *user, usize size,
                                                VkInternalAllocationType type,
                                                VkSystemAllocationScope  scope);

      private:
        TlsfAllocator        *m_tlsf = nullptr;
        VkAllocationCallbacks m_callbacks = {};
    };

} // namespace mantle

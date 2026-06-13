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

#include "vulkan_cpu_allocator.h"

#include "mantle/core/assert.h"

namespace mantle {

    void VulkanCPUAllocator::init(TlsfAllocator *tlsf) {
        MANTLE_CHECK(tlsf != nullptr);

        m_tlsf = tlsf;
        m_callbacks.pUserData = this;
        m_callbacks.pfnAllocation = vk_alloc;
        m_callbacks.pfnReallocation = vk_realloc;
        m_callbacks.pfnFree = vk_free;
        m_callbacks.pfnInternalAllocation = vk_internal_alloc;
        m_callbacks.pfnInternalFree = vk_internal_free;
    }

    VkAllocationCallbacks *VulkanCPUAllocator::vk_allocator() { return &m_callbacks; }

    void *VKAPI_CALL VulkanCPUAllocator::vk_alloc(void *user, usize size, usize align,
                                                  VkSystemAllocationScope scope) {
        if (size == 0) {
            return nullptr;
        }
        auto *self = static_cast<VulkanCPUAllocator *>(user);
        return self->m_tlsf->alloc(size, align);
    }

    void *VKAPI_CALL VulkanCPUAllocator::vk_realloc(void *user, void *original, usize size,
                                                    usize align, VkSystemAllocationScope scope) {
        if (size == 0) {
            if (original) {
                auto *self = static_cast<VulkanCPUAllocator *>(user);
                self->m_tlsf->free(original);
            }
            return nullptr;
        }

        auto *self = static_cast<VulkanCPUAllocator *>(user);

        if (original == nullptr) {
            return self->m_tlsf->alloc(size, align);
        }

        return self->m_tlsf->realloc(original, size);
    }

    void VKAPI_CALL VulkanCPUAllocator::vk_free(void *user, void *memory) {
        if (memory == nullptr) {
            return;
        }
        auto *self = static_cast<VulkanCPUAllocator *>(user);
        self->m_tlsf->free(memory);
    }

    void VKAPI_CALL VulkanCPUAllocator::vk_internal_alloc(void *user, usize size,
                                                          VkInternalAllocationType type,
                                                          VkSystemAllocationScope  scope) {}

    void VKAPI_CALL VulkanCPUAllocator::vk_internal_free(void *user, usize size,
                                                         VkInternalAllocationType type,
                                                         VkSystemAllocationScope  scope) {}

} // namespace mantle

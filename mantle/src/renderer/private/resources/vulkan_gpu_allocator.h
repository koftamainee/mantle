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

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "mantle/core/macros.h"
#include "mantle/core/types.h"

namespace spdlog {
    class logger;
}

namespace mantle {
    class VulkanGPUAllocator final {
      public:
        MANTLE_DEFAULT_INIT(VulkanGPUAllocator);

        void init(VkPhysicalDevice physical_device, VkDevice device, VkInstance instance,
                  VkAllocationCallbacks *vk_callbacks);
        void destroy();

        VkResult create_buffer(VkDeviceSize size, VkBufferUsageFlags usage,
                               VmaMemoryUsage memory_usage, VkBuffer *buffer,
                               VmaAllocation *allocation, void **mapped_data = nullptr) const;

        void destroy_buffer(VkBuffer buffer, VmaAllocation allocation) const;

        VkResult map_memory(VmaAllocation allocation, void **data) const;
        void     unmap_memory(VmaAllocation allocation) const;


        VkResult create_image(const VkImageCreateInfo &image_info, VmaMemoryUsage memory_usage,
                              VkImage *image, VmaAllocation *allocation) const;

        void destroy_image(VkImage image, VmaAllocation allocation) const;


        VkDeviceMemory get_allocation_memory(VmaAllocation allocation) const;
        VkDeviceSize   get_allocation_size(VmaAllocation allocation) const;

      private:
        VmaAllocator m_allocator {};
        bool         m_is_initialized = false;

        spdlog::logger *m_logger = nullptr;
    };
} // namespace mantle

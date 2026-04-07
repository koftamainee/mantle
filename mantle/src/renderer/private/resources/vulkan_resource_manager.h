#pragma once
#include <vma/vk_mem_alloc.h>
#include "core/types.h"

#include "deletion_queue.h"
#include "vulkan_allocator.h"

#include <array>

namespace mantle {
    class VulkanResourceManager final {
      public:
        using ResourceID = u32;

        enum class ResourceType { Buffer, Image, Shader, Sampler };

        struct ResourceHandle {
            ResourceID id = 0;
            ResourceType type{};
            u32 generation = 0;
        };

        VulkanResourceManager() = default;
        ~VulkanResourceManager();

        VulkanResourceManager(const VulkanResourceManager &) = delete;
        VulkanResourceManager &
        operator=(const VulkanResourceManager &) = delete;
        VulkanResourceManager(VulkanResourceManager &&) noexcept = delete;
        VulkanResourceManager &
        operator=(VulkanResourceManager &&) noexcept = delete;

        void init(VkPhysicalDevice physical_device, VkDevice device,
                  VkInstance instance);
        void destroy();

        ResourceHandle create_buffer(VkDeviceSize size,
                                     VkBufferUsageFlags usage,
                                     VmaMemoryUsage memory_usage,
                                     void **mapped_data = nullptr);
        void destroy_buffer(ResourceHandle handle, bool immediate = false);
        VkBuffer get_buffer(ResourceHandle handle) const;

        ResourceHandle create_image(const VkImageCreateInfo &info,
                                    VmaMemoryUsage memory_usage);
        void destroy_image(ResourceHandle handle, bool immediate = false);
        VkImage get_image(ResourceHandle handle) const;

        void next_frame();

      private:
        bool m_is_initialized = false;

        static constexpr u32 ms_frame_lag = 3;
        u32 m_current_frame = 0;
        std::array<DeletionQueue, ms_frame_lag> m_deletion_queues{};

        VulkanAllocator m_allocator;

        struct BufferData {
            VkBuffer buffer;
            VmaAllocation allocation;
        };

        struct ImageData {
            VkImage image;
            VmaAllocation allocation;
        };

        std::vector<BufferData> m_buffers;
        std::vector<ImageData> m_images;

        std::vector<ResourceID> m_image_free_list;
        std::vector<ResourceID> m_buffer_free_list;

        std::vector<u32> m_buffer_generations;
        std::vector<u32> m_image_generations;

        template <typename TData>
        TData &get_resource_data(ResourceHandle handle,
                                 ResourceType expected_type,
                                 std::vector<TData> &storage,
                                 const std::vector<u32> &generations);

        template <typename TData>
        const TData &
        get_resource_data(ResourceHandle handle, ResourceType expected_type,
                          const std::vector<TData> &storage,
                          const std::vector<u32> &generations) const;
    };
} // namespace mantle

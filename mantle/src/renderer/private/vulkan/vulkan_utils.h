#pragma once
#include <string_view>
#include <vector>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include "core/types.h"
#include "renderer/types.h"
#include "types.h"

namespace mantle {
    void load_spv(std::string_view path, std::pmr::vector<u32> &out);

    ImageFormat from_vk(VkFormat format);
    VkFormat to_vk(ImageFormat format);
    VkImageUsageFlags to_vk(ImageUsage usage);
    VkImageAspectFlags to_vk_aspect(ImageFormat format);
    VkBufferUsageFlags to_vk(BufferUsage usage);
    VmaMemoryUsage to_vma(MemoryType type);
    VkFilter to_vk(Filter filter);
    VkSamplerMipmapMode to_vk_mip(Filter filter);
    VkSamplerAddressMode to_vk(AddressMode mode);
    VkAttachmentLoadOp to_vk(AttachmentLoad load);
    VkAttachmentStoreOp to_vk(AttachmentStore store);

    VkImageLayout to_vk(ImageLayout layout);
    VkPipelineStageFlags2 to_vk(PipelineStage stage);
    VkAccessFlags2 infer_access(ImageLayout layout, bool is_src);
    ImageLayout infer_layout(RGAccessType access, ImageUsage usage);
    PipelineStage infer_stage(RGAccessType access, ImageUsage usage);
    VkSampleCountFlagBits to_vk(SampleCount count);
} // namespace mantle

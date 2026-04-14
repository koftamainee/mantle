#include "../vulkan/vulkan_utils.h"
#include "types.h"

#include <fstream>
#include <ios>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include "core/assert.h"
#include "renderer/types.h"

namespace mantle {

    void load_spv(std::string_view path, std::pmr::vector<u32> &out) {
        std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
        fatal(!file.is_open(), "failed to open shader file");

        long size = file.tellg();
        fatal(size % 4 != 0, "Invalid SPIR-V shader");

        file.seekg(0);
        out.resize(size / 4);
        file.read(reinterpret_cast<char *>(out.data()), size);
    }

    ImageFormat from_vk(VkFormat format) {
        switch (format) {
        case VK_FORMAT_R8G8B8A8_UNORM:
            return ImageFormat::Rgba8;
        case VK_FORMAT_R8G8B8A8_SRGB:
            return ImageFormat::Rgba8Srgb;
        case VK_FORMAT_B8G8R8A8_SRGB:
            return ImageFormat::Bgra8Srgb;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return ImageFormat::Rgba16f;
        case VK_FORMAT_R32_SFLOAT:
            return ImageFormat::R32f;
        case VK_FORMAT_D16_UNORM:
            return ImageFormat::D16;
        case VK_FORMAT_D32_SFLOAT:
            return ImageFormat::D32;
        case VK_FORMAT_D16_UNORM_S8_UINT:
            return ImageFormat::D16S8;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return ImageFormat::D24S8;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return ImageFormat::D32S8;
        default:
            fatal(true, "unsupported VkFormat");
        }
    }

    VkFormat to_vk(ImageFormat format) {
        switch (format) {
        case ImageFormat::Rgba8:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::Rgba8Srgb:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case ImageFormat::Bgra8Srgb:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case ImageFormat::Rgba16f:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case ImageFormat::R32f:
            return VK_FORMAT_R32_SFLOAT;
        case ImageFormat::D16:
            return VK_FORMAT_D16_UNORM;
        case ImageFormat::D32:
            return VK_FORMAT_D32_SFLOAT;
        case ImageFormat::D16S8:
            return VK_FORMAT_D16_UNORM_S8_UINT;
        case ImageFormat::D24S8:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case ImageFormat::D32S8:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;

        default:
            fatal(true, "unsupported ImageFormat");
        }
    }

    VkImageUsageFlags to_vk(ImageUsage usage) {
        using U = std::underlying_type_t<ImageUsage>;
        U u = static_cast<U>(usage);

        VkImageUsageFlags flags = 0;

        auto has = [&](ImageUsage bit) {
            return (u & static_cast<U>(bit)) != 0;
        };

        if (has(ImageUsage::Sampled))
            flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (has(ImageUsage::Storage))
            flags |= VK_IMAGE_USAGE_STORAGE_BIT;
        if (has(ImageUsage::Color))
            flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (has(ImageUsage::Depth))
            flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        if (has(ImageUsage::TransferSrc))
            flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (has(ImageUsage::TransferDst))
            flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        return flags;
    }

    VkImageAspectFlags to_vk_aspect(ImageFormat format) {
        switch (format) {
        case ImageFormat::D16:
        case ImageFormat::D32:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case ImageFormat::D16S8:
        case ImageFormat::D24S8:
        case ImageFormat::D32S8:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    VkBufferUsageFlags to_vk(BufferUsage usage) {
        using U = std::underlying_type_t<BufferUsage>;
        U u = static_cast<U>(usage);

        VkBufferUsageFlags flags = 0;

        auto has = [&](BufferUsage bit) {
            return (u & static_cast<U>(bit)) != 0;
        };

        if (has(BufferUsage::Vertex))
            flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (has(BufferUsage::Index))
            flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (has(BufferUsage::Uniform))
            flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (has(BufferUsage::Storage))
            flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (has(BufferUsage::Transfer))
            flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        return flags;
    }

    VmaMemoryUsage to_vma(MemoryType type) {
        switch (type) {
        case MemoryType::Gpu:
            return VMA_MEMORY_USAGE_GPU_ONLY;
        case MemoryType::CpuToGpu:
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        default:
            fatal(true, "unsupported MemoryType");
        }
    }

    VkFilter to_vk(Filter filter) {
        switch (filter) {
        case Filter::Nearest:
            return VK_FILTER_NEAREST;
        case Filter::Linear:
            return VK_FILTER_LINEAR;
        default:
            fatal(true, "unsupported Filter");
        }
    }

    VkSamplerMipmapMode to_vk_mip(Filter filter) {
        switch (filter) {
        case Filter::Nearest:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case Filter::Linear:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default:
            fatal(true, "unsupported Filter");
        }
    }

    VkSamplerAddressMode to_vk(AddressMode mode) {
        switch (mode) {
        case AddressMode::Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case AddressMode::ClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case AddressMode::ClampToBorder:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default:
            fatal(true, "unsupported AddressMode");
        }
    }

    VkAttachmentLoadOp to_vk(AttachmentLoad load) {
        switch (load) {
        case AttachmentLoad::Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case AttachmentLoad::Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case AttachmentLoad::DontCare:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default:
            fatal(true, "unsupported AttachmentLoad");
        }
    }

    VkAttachmentStoreOp to_vk(AttachmentStore store) {
        switch (store) {
        case AttachmentStore::Store:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case AttachmentStore::DontCare:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default:
            fatal(true, "unsupported AttachmentStore");
        }
    }
    VkImageLayout to_vk(ImageLayout layout) {
        switch (layout) {
        case ImageLayout::Undefined:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        case ImageLayout::General:
            return VK_IMAGE_LAYOUT_GENERAL;
        case ImageLayout::ColorAttachment:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case ImageLayout::DepthAttachment:
            return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        case ImageLayout::ShaderReadOnly:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case ImageLayout::TransferSrc:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case ImageLayout::TransferDst:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case ImageLayout::Present:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default:
            fatal(true, "unsupported ImageLayout");
        }
    }

    VkPipelineStageFlags2 to_vk(PipelineStage stage) {
        using U = std::underlying_type_t<PipelineStage>;
        U u = static_cast<U>(stage);

        VkPipelineStageFlags2 flags = 0;

        auto has = [&](PipelineStage bit) {
            return (u & static_cast<U>(bit)) != 0;
        };

        if (has(PipelineStage::None))
            flags |= VK_PIPELINE_STAGE_2_NONE;
        if (has(PipelineStage::Top))
            flags |= VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
        if (has(PipelineStage::Bottom))
            flags |= VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        if (has(PipelineStage::AllCommands))
            flags |= VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        if (has(PipelineStage::ComputeShader))
            flags |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        if (has(PipelineStage::VertexShader))
            flags |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
        if (has(PipelineStage::FragmentShader))
            flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        if (has(PipelineStage::ColorOutput))
            flags |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        if (has(PipelineStage::EarlyDepth))
            flags |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
        if (has(PipelineStage::LateDepth))
            flags |= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        if (has(PipelineStage::Transfer))
            flags |= VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT;

        return flags;
    }

    VkAccessFlags2 infer_access(ImageLayout layout, bool is_src) {
        switch (layout) {
        case ImageLayout::ColorAttachment:
            return is_src ? VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT
                          : VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
                    VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        case ImageLayout::DepthAttachment:
            return is_src ? VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
                          : VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                    VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        case ImageLayout::ShaderReadOnly:
            return VK_ACCESS_2_SHADER_READ_BIT;
        case ImageLayout::General:
            return VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT;
        case ImageLayout::TransferSrc:
            return VK_ACCESS_2_TRANSFER_READ_BIT;
        case ImageLayout::TransferDst:
            return VK_ACCESS_2_TRANSFER_WRITE_BIT;
        case ImageLayout::Present:
        case ImageLayout::Undefined:
            return VK_ACCESS_2_NONE;
        default:
            fatal(true, "unsupported ImageLayout");
        }
    }

    ImageLayout infer_layout(RGAccessType access, ImageUsage usage) {
        using U = std::underlying_type_t<ImageUsage>;
        U u = static_cast<U>(usage);

        auto has = [&](ImageUsage bit) {
            return (u & static_cast<U>(bit)) != 0;
        };

        if (has(ImageUsage::TransferSrc) && access == RGAccessType::Read)
            return ImageLayout::TransferSrc;

        if (has(ImageUsage::TransferDst) && access == RGAccessType::Write)
            return ImageLayout::TransferDst;

        if (has(ImageUsage::Depth))
            return ImageLayout::DepthAttachment;

        if (has(ImageUsage::Color) && access == RGAccessType::Write)
            return ImageLayout::ColorAttachment;

        if (has(ImageUsage::Storage))
            return ImageLayout::General;

        if (has(ImageUsage::Sampled) && access == RGAccessType::Read)
            return ImageLayout::ShaderReadOnly;

        if (has(ImageUsage::Color) && access == RGAccessType::Read)
            return ImageLayout::ShaderReadOnly;

        fatal(true, "cannot infer ImageLayout");
    }

    PipelineStage infer_stage(RGAccessType access, ImageUsage usage) {
        using U = std::underlying_type_t<ImageUsage>;
        U u = static_cast<U>(usage);

        auto has = [&](ImageUsage bit) {
            return (u & static_cast<U>(bit)) != 0;
        };

        if (has(ImageUsage::TransferSrc) || has(ImageUsage::TransferDst))
            return PipelineStage::Transfer;

        if (has(ImageUsage::Depth))
            return access == RGAccessType::Read
                ? PipelineStage::EarlyDepth
                : static_cast<PipelineStage>(
                      static_cast<u32>(PipelineStage::EarlyDepth) |
                      static_cast<u32>(PipelineStage::LateDepth));

        if (has(ImageUsage::Color) && access == RGAccessType::Write)
            return PipelineStage::ColorOutput;

        if (has(ImageUsage::Storage) || has(ImageUsage::Sampled) ||
            has(ImageUsage::Color))
            return PipelineStage::FragmentShader;

        fatal(true, "cannot infer PipelineStage");
    }

    VkSampleCountFlagBits to_vk(SampleCount count) {
        switch (count) {
        case SampleCount::x1:
            return VK_SAMPLE_COUNT_1_BIT;
        case SampleCount::x2:
            return VK_SAMPLE_COUNT_2_BIT;
        case SampleCount::x4:
            return VK_SAMPLE_COUNT_4_BIT;
        case SampleCount::x8:
            return VK_SAMPLE_COUNT_8_BIT;
        }
        fatal(true, "Unknown sample count");
    }

} // namespace mantle

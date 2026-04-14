#pragma once
#include <vulkan/vulkan.h>
#include "core/types.h"
#include "renderer/types.h"
#include "core/enum_flags.h"

namespace mantle {

    enum class RGResourceType : u8 {
        Image,
        Buffer,
    };

    enum class RGAccessType : u8 {
        None,
        Read,
        Write,
        ReadWrite,
    };

    enum class ImageLayout : u8 {
        Undefined,
        General,
        ColorAttachment,
        DepthAttachment,
        ShaderReadOnly,
        TransferSrc,
        TransferDst,
        Present,
    };

    enum class PipelineStage : u32 {
        None = 0,
        Top = 1 << 0,
        Bottom = 1 << 1,
        AllCommands = 1 << 2,
        ComputeShader = 1 << 3,
        VertexShader = 1 << 4,
        FragmentShader = 1 << 5,
        ColorOutput = 1 << 6,
        EarlyDepth = 1 << 7,
        LateDepth = 1 << 8,
        Transfer = 1 << 9,
        MaxEnum = (1 << 10) - 1,
    };

    struct ImageBarrier final {
        ImageHandle image = {};
        ImageLayout from = ImageLayout::Undefined;
        ImageLayout to = ImageLayout::Undefined;
        PipelineStage src_stage = PipelineStage::None;
        PipelineStage dst_stage = PipelineStage::None;
    };

    struct BufferBarrier final {
        BufferHandle buffer = {};
        PipelineStage src_stage = PipelineStage::None;
        PipelineStage dst_stage = PipelineStage::None;
        VkAccessFlags2 src_access = 0;
        VkAccessFlags2 dst_access = 0;
    };

    struct RGImageResource final {
        ImageHandle physical = {};
        ImageDesc desc = {};
        ImageLayout layout = ImageLayout::Undefined;
        RGAccessType access = RGAccessType::None;
        bool imported = false;
    };

    struct RGBufferResource final {
        BufferHandle physical = {};
        BufferDesc desc = {};
        RGAccessType access = RGAccessType::None;
        bool imported = false;
    };

    struct RGResourceAccess final {
        u32 index = 0;
        RGResourceType type = {};
        RGAccessType access = RGAccessType::None;
    };

} // namespace mantle

#pragma once
#include "core/enum_flags.h"
#include <span>

namespace mantle {
    struct BufferHandle final {
        u32 index;
        u32 generation;
    };
    struct ImageHandle final {
        u32 index;
        u32 generation;
    };
    struct SamplerHandle final {
        u32 index;
        u32 generation;
    };
    struct ShaderHandle final {
        u32 index;
        u32 generation;
    };
    struct GraphicsPipelineHandle final {
        u32 index;
        u32 generation;
    };
    struct ComputePipelineHandle final {
        u32 index;
        u32 generation;
    };

    struct RGBufferHandle final {
        u32 index;
    };
    struct RGImageHandle final {
        u32 index;
    };

    enum class BufferUsage : u32 {
        Vertex = 1 << 0,
        Index = 1 << 1,
        Uniform = 1 << 2,
        Storage = 1 << 3,
        Transfer = 1 << 4,
    };
    enum class MemoryType {
        Gpu,
        CpuToGpu,
    };
    struct BufferDesc final {
        usize size = 0;
        BufferUsage usage = {};
        MemoryType memory = MemoryType::Gpu;
    };

    enum class ImageFormat {
        Rgba8,
        Rgba8Srgb,
        Bgra8Srgb,
        Rgba16f,
        R32f,
        D16,
        D32,
        D16S8,
        D24S8,
        D32S8,
    };
    enum class ImageUsage : u32 {
        None = 0,
        Sampled = 1 << 0,
        Storage = 1 << 1,
        Color = 1 << 2,
        Depth = 1 << 3,
        TransferSrc = 1 << 4,
        TransferDst = 1 << 5,
        MaxEnum = (1 << 6) - 1,
    };
    enum class SampleCount : u8 {
        x1 = 1,
        x2 = 2,
        x4 = 4,
        x8 = 8,
    };
    struct ImageDesc final {
        u32 width = 0;
        u32 height = 0;
        u32 depth = 1;
        u32 mip_levels = 1;
        u32 array_layers = 1;
        SampleCount sample_count = SampleCount::x1;
        ImageFormat format = {};
        ImageUsage usage = {};
        bool create_view = true;
    };

    struct GraphicsPipelineDesc final {}; // TODO
    struct ComputePipelineDesc final {}; // TODO

    enum class Filter {
        Nearest,
        Linear,
    };
    enum class AddressMode {
        Repeat,
        ClampToEdge,
        ClampToBorder,
    };
    struct SamplerDesc final {
        Filter min_filter = Filter::Linear;
        Filter mag_filter = Filter::Linear;
        Filter mip_filter = Filter::Linear;
        AddressMode address_u = AddressMode::Repeat;
        AddressMode address_v = AddressMode::Repeat;
        f32 max_anisotropy = 1.0f;
        f32 min_lod = 0.0f;
        f32 max_lod = 1.0f;
    };

    enum class AttachmentLoad { Clear, Load, DontCare };
    enum class AttachmentStore { Store, DontCare };

    struct RGColorAttachment final {
        RGImageHandle image = {};
        AttachmentLoad load = AttachmentLoad::Clear;
        AttachmentStore store = AttachmentStore::Store;
        f32 clear_r = 0.0f;
        f32 clear_g = 0.0f;
        f32 clear_b = 0.0f;
        f32 clear_a = 1.0f;
    };

    struct RGDepthAttachment final {
        RGImageHandle image = {};
        AttachmentLoad load = AttachmentLoad::Clear;
        AttachmentStore store = AttachmentStore::DontCare;
        f32 clear_value = 1.0f;
    };

    struct RGRenderingInfo final {
        std::span<RGColorAttachment> colors = {};
        RGDepthAttachment depth = {};
        u32 width = 0;
        u32 height = 0;
    };

    struct DrawInfo final {
        u32 vertex_count = 0;
        u32 instance_count = 1;
        u32 first_vertex = 0;
        u32 first_instance = 0;
    };

    struct DrawIndexedInfo final {
        u32 index_count = 0;
        u32 instance_count = 1;
        u32 first_index = 0;
        i32 vertex_offset = 0;
        u32 first_instance = 0;
    };

    struct DispatchInfo final {
        u32 x = 1;
        u32 y = 1;
        u32 z = 1;
    };

    struct RGBufferCopyInfo final {
        RGBufferHandle src = {};
        RGBufferHandle dst = {};
        usize src_offset = 0;
        usize dst_offset = 0;
        usize size = 0;
    };

    struct RGBufferImageCopyInfo final {
        RGBufferHandle src = {};
        RGImageHandle dst = {};
        usize buffer_offset = 0;
        u32 mip_level = 0;
    };

} // namespace mantle

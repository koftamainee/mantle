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

#include "mantle/renderer/types.h"
#include "types.h"
#include "vulkan_gpu_allocator.h"

namespace mantle {
    struct BufferResource final {
        VkBuffer      buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        void         *mapped = nullptr;
        BufferDesc    desc = {};

        PipelineStage current_stage = PipelineStage::Top;
        AccessType    current_access = AccessType::None;

        u32 bindless_index = UINT32_MAX;
    };
    struct ImageResource final {
        VkImage       image = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        VkImageView   view = VK_NULL_HANDLE;
        ImageDesc     desc = {};

        ImageLayout current_layout = ImageLayout::Undefined;

        u32 bindless_sample_index = UINT32_MAX;
        u32 bindless_storage_index = UINT32_MAX;
    };
    struct SamplerResource final {
        VkSampler   sampler = VK_NULL_HANDLE;
        SamplerDesc desc = {};

        u32 bindless_index = UINT32_MAX;
    };
    struct ShaderResource final {
        VkShaderModule shader = VK_NULL_HANDLE;
    };
    struct GraphicsPipelineResource final {
        VkPipeline           pipeline = VK_NULL_HANDLE;
        VkPipelineLayout     layout = VK_NULL_HANDLE;
        GraphicsPipelineDesc desc = {};
    };
    struct ComputePipelineResource final {
        VkPipeline          pipeline = VK_NULL_HANDLE;
        VkPipelineLayout    layout = VK_NULL_HANDLE;
        ComputePipelineDesc desc = {};
    };
} // namespace mantle

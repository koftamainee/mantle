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

#include "mantle/core/assert.h"
#include "types.h"

namespace mantle {

    inline ImageLayout write_usage_to_layout(WriteUsage usage) {
        auto result = ImageLayout::Undefined;

        switch (usage) {
            case WriteUsage::ColorAttachment: {
                result = ImageLayout::ColorAttachment;
            } break;
            case WriteUsage::DepthAttachment: {
                result = ImageLayout::DepthAttachment;
            } break;
            case WriteUsage::StorageWrite: {
                result = ImageLayout::General;
            } break;
            case WriteUsage::TransferDst: {
                result = ImageLayout::TransferDst;
            } break;
            case WriteUsage::Clear: {
                result = ImageLayout::General;
            } break;
            case WriteUsage::Present: {
                result = ImageLayout::Present;
            } break;
            default: {
                MANTLE_FATAL(true, "unknown WriteUsage");
            } break;
        }

        return result;
    }

    inline PipelineStage write_usage_to_stage(WriteUsage usage) {
        auto result = PipelineStage::Top;

        switch (usage) {
            case WriteUsage::ColorAttachment: {
                result = PipelineStage::ColorOutput;
            } break;
            case WriteUsage::DepthAttachment: {
                result = PipelineStage::EarlyDepth;
            } break;
            case WriteUsage::StorageWrite: {
                result = PipelineStage::ComputeShader;
            } break;
            case WriteUsage::TransferDst: {
                result = PipelineStage::Transfer;
            } break;
            case WriteUsage::Clear: {
                result = PipelineStage::Transfer;
            } break;
            case WriteUsage::Present: {
                result = PipelineStage::Bottom;
            } break;
            default: {
                MANTLE_FATAL(true, "unknown WriteUsage");
            } break;
        }

        return result;
    }

    inline AccessType write_usage_to_access(WriteUsage usage) {
        auto result = AccessType::None;

        switch (usage) {
            case WriteUsage::ColorAttachment: {
                result = AccessType::ColorAttachmentWrite;
            } break;
            case WriteUsage::DepthAttachment: {
                result = AccessType::DepthAttachmentWrite;
            } break;
            case WriteUsage::StorageWrite: {
                result = AccessType::ShaderWrite;
            } break;
            case WriteUsage::TransferDst: {
                result = AccessType::TransferWrite;
            } break;
            case WriteUsage::Clear: {
                result = AccessType::TransferWrite;
            } break;
            case WriteUsage::Present: {
                result = AccessType::None;
            } break;
            default: {
                MANTLE_FATAL(true, "unknown WriteUsage");
            } break;
        }

        return result;
    }

    inline ImageLayout read_usage_to_layout(ReadUsage usage) {
        auto result = ImageLayout::Undefined;

        switch (usage) {
            case ReadUsage::Sampled: {
                result = ImageLayout::ShaderReadOnly;
            } break;
            case ReadUsage::StorageRead: {
                result = ImageLayout::General;
            } break;
            case ReadUsage::InputAttachment: {
                result = ImageLayout::ShaderReadOnly;
            } break;
            case ReadUsage::TransferSrc: {
                result = ImageLayout::TransferSrc;
            } break;
            case ReadUsage::IndirectArg: {
                result = ImageLayout::ShaderReadOnly;
            } break;
            default: {
                MANTLE_FATAL(true, "unknown ReadUsage");
            } break;
        }

        return result;
    }

    inline PipelineStage read_usage_to_stage(ReadUsage usage) {
        auto result = PipelineStage::Top;

        switch (usage) {
            case ReadUsage::Sampled: {
                result = PipelineStage::FragmentShader;
            } break;
            case ReadUsage::StorageRead: {
                result = PipelineStage::ComputeShader;
            } break;
            case ReadUsage::InputAttachment: {
                result = PipelineStage::FragmentShader;
            } break;
            case ReadUsage::TransferSrc: {
                result = PipelineStage::Transfer;
            } break;
            case ReadUsage::IndirectArg: {
                result = PipelineStage::DrawIndirect;
            } break;
            default: {
                MANTLE_FATAL(true, "unknown ReadUsage");
            } break;
        }

        return result;
    }

    inline AccessType read_usage_to_access(ReadUsage usage) {
        auto result = AccessType::None;

        switch (usage) {
            case ReadUsage::Sampled: {
                result = AccessType::ShaderRead;
            } break;
            case ReadUsage::StorageRead: {
                result = AccessType::ShaderRead;
            } break;
            case ReadUsage::InputAttachment: {
                result = AccessType::ShaderRead;
            } break;
            case ReadUsage::TransferSrc: {
                result = AccessType::TransferRead;
            } break;
            default: {
                MANTLE_FATAL(true, "unknown ReadUsage");
            } break;
        }

        return result;
    }

    inline PipelineStage buffer_read_usage_to_stage(BufferReadUsage usage) {
        auto result = PipelineStage::Top;

        switch (usage) {
            case BufferReadUsage::Vertex:
            case BufferReadUsage::Index: {
                result = PipelineStage::VertexInput;
            } break;
            case BufferReadUsage::Uniform: {
                result = PipelineStage::FragmentShader;
            } break;
            case BufferReadUsage::Storage: {
                result = PipelineStage::ComputeShader;
            } break;
            case BufferReadUsage::TransferSrc: {
                result = PipelineStage::Transfer;
            } break;
            case BufferReadUsage::IndirectArg: {
                result = PipelineStage::DrawIndirect;
            } break;
            default: {
                MANTLE_FATAL(true, "unknown BufferReadUsage");
            } break;
        }

        return result;
    }

    inline AccessType buffer_read_usage_to_access(BufferReadUsage usage) {
        auto result = AccessType::None;

        switch (usage) {
            case BufferReadUsage::Vertex: {
                result = AccessType::VertexAttributeRead;
            } break;
            case BufferReadUsage::Index: {
                result = AccessType::IndexRead;
            } break;
            case BufferReadUsage::Uniform: {
                result = AccessType::UniformRead;
            } break;
            case BufferReadUsage::Storage: {
                result = AccessType::ShaderRead;
            } break;
            case BufferReadUsage::TransferSrc: {
                result = AccessType::TransferRead;
            } break;
            case BufferReadUsage::IndirectArg: {
                result = AccessType::IndirectCommandRead;
            } break;
            default: {
                MANTLE_FATAL(true, "unknown BufferReadUsage");
            } break;
        }

        return result;
    }

    inline PipelineStage buffer_write_usage_to_stage(BufferWriteUsage usage) {
        auto result = PipelineStage::Top;

        switch (usage) {
            case BufferWriteUsage::Storage: {
                result = PipelineStage::ComputeShader;
            } break;
            case BufferWriteUsage::TransferDst: {
                result = PipelineStage::Transfer;
            } break;
            default: {
                MANTLE_FATAL(true, "unknown BufferWriteUsage");
            } break;
        }

        return result;
    }

    inline AccessType buffer_write_usage_to_access(BufferWriteUsage usage) {
        auto result = AccessType::None;

        switch (usage) {
            case BufferWriteUsage::Storage: {
                result = AccessType::ShaderWrite;
            } break;
            case BufferWriteUsage::TransferDst: {
                result = AccessType::TransferWrite;
            } break;
            default: {
                MANTLE_FATAL(true, "unknown BufferWriteUsage");
            } break;
        }

        return result;
    }

} // namespace mantle

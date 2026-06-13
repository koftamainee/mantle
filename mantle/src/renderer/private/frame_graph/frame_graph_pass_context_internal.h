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

#include <vulkan/command_recorder.h>

#include "mantle/core/memory/arena_allocator.h"
#include "mantle/core/memory/pmr/arena_resource.h"
#include "mantle/renderer/frame_graph.h"
#include "mantle/renderer/types.h"
#include "resources/gpu_resource_manager_internal.h"
#include "resources/transient_resources.h"

namespace mantle {
    struct FGPassContext::Impl {
        CommandRecorder *cmd;

        GPUResourceManager *resource_manager = nullptr;

        TransientResources *transient_resources = nullptr;

        ArenaAllocator *scratch_arena = nullptr;
        ArenaResource  *scratch_resource = nullptr;
    };
} // namespace mantle

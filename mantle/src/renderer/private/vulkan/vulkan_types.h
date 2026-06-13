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

#include <vulkan/vulkan.h>

#include <vector>

#include "mantle/core/types.h"

namespace mantle {

    struct QueueFamilyIndices {
        u32 graphics_family = UINT32_MAX;
        u32 present_family = UINT32_MAX;
        u32 transfer_family = UINT32_MAX;

        bool is_complete() const {
            return graphics_family != UINT32_MAX && present_family != UINT32_MAX &&
                   transfer_family != UINT32_MAX;
        }
    };

    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR             capabilities;
        std::pmr::vector<VkSurfaceFormatKHR> formats;
        std::pmr::vector<VkPresentModeKHR>   present_modes;
    };

} // namespace mantle

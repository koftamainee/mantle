#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "core/types.h"

namespace mantle {

    struct QueueFamilyIndices {
        u32 graphics_family = UINT32_MAX;
        u32 present_family = UINT32_MAX;
        u32 transfer_family = UINT32_MAX;

        bool is_complete() const {
            return graphics_family != UINT32_MAX
                && present_family != UINT32_MAX
                && transfer_family != UINT32_MAX;
        }
    };

    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

} // namespace mantle

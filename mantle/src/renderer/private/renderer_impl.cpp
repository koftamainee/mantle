#include "renderer_impl.h"

#include <spdlog/spdlog.h>
#include "vkassert.h"
#include "core/assert.h"

namespace mantle {
    void Renderer::Impl::create_frame(FrameData &frame) const {
        VkDevice vkdevice = device.get_device();

        VkFenceCreateInfo fence_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        vk_verify(vkCreateFence(vkdevice, &fence_info, nullptr, &frame.in_flight));

        frame.cmd = device.create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, false);
    }

    void Renderer::Impl::destroy_frame(FrameData &frame) const {
        VkDevice vkdevice = device.get_device();
        if (frame.in_flight != VK_NULL_HANDLE) {
            vkDestroyFence(vkdevice, frame.in_flight, nullptr);
        }
        frame = {};
    }

    void Renderer::Impl::create_frames() {
        frames.resize(frames_in_flight);
        for (auto &frame : frames) {
            create_frame(frame);
        }

        uint32_t image_count = static_cast<uint32_t>(swapchain.get_images().size());

        acquire_semaphores.resize(image_count);
        render_semaphores.resize(image_count);

        VkSemaphoreCreateInfo sem_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        for (auto &sem : acquire_semaphores) {
            vk_verify(vkCreateSemaphore(device.get_device(), &sem_info, nullptr, &sem));
        }
        for (auto &sem : render_semaphores) {
            vk_verify(vkCreateSemaphore(device.get_device(), &sem_info, nullptr, &sem));
        }

        spdlog::info("FrameData object are created");
    }

    void Renderer::Impl::destroy_frames() {
        for (auto &frame : frames) {
            destroy_frame(frame);
        }
        frames.clear();

        for (auto &sem : acquire_semaphores) {
            if (sem != VK_NULL_HANDLE) {
                vkDestroySemaphore(device.get_device(), sem, nullptr);
            }
        }
        acquire_semaphores.clear();

        for (auto &sem : render_semaphores) {
            if (sem != VK_NULL_HANDLE) {
                vkDestroySemaphore(device.get_device(), sem, nullptr);
            }
        }
        render_semaphores.clear();

        spdlog::info("FrameData objects are destroyed");
    }

    FrameData &Renderer::Impl::get_current_frame() {
        return frames[current_frame];
    }
}
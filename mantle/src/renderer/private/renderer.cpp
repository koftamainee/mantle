#include <renderer/renderer.h>
#include <vulkan_graphics_context.h>
#include <vulkan_device.h>
#include <vulkan_swapchain.h>
#include <core/assert.h>
#include <spdlog/spdlog.h>

#include "vulkan_allocator.h"
#include "window/window.h"

namespace mantle {
    struct Renderer::Impl {
        VulkanGraphicsContext graphics_context;
        VulkanDevice device;
        VulkanAllocator allocator;
        VulkanSwapchain swapchain;
    };

    Renderer::Renderer() = default;
    Renderer::~Renderer() { destroy(); }

    void Renderer::init(const Window &window) {
        check(!m_is_initialized);

        m_impl = std::make_unique<Impl>();

        m_impl->graphics_context.init(window.get_native_window());
        VkInstance instance = m_impl->graphics_context.get_instance();
        VkSurfaceKHR surface = m_impl->graphics_context.get_surface();


        m_impl->device.init(m_impl->graphics_context.get_instance(), surface);
        VkDevice device = m_impl->device.get_device();
        VkPhysicalDevice physical_device = m_impl->device.get_physical_device();

        m_impl->allocator.init(physical_device, device, instance);

        auto [width, height] = window.get_size();

        m_impl->swapchain.init(device, surface,
                               m_impl->device.get_swapchain_support_details(surface),
                               m_impl->device.get_queue_families(), width, height);

        m_is_initialized = true;
        spdlog::info("Renderer Initialized");
    }

    void Renderer::destroy() {
        if (m_is_initialized) {
            m_impl->swapchain.destroy();
            m_impl->allocator.destroy();
            m_impl->device.destroy();
            m_impl->graphics_context.destroy();

            m_impl.reset();

            spdlog::info("Renderer Destroyed");
            m_is_initialized = false;
        }
    }
} // namespace VkEngine

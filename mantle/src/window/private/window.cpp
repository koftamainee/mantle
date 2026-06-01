#include <GLFW/glfw3.h>
#include <array>
#include <utility>
#include <window/window.h>

#include <core/assert.h>

#include "../public/window/glfw_custom_allocator.h"
#include "core/memory/memory_units.h"
#include "core/memory/tlsf_allocator.h"


namespace mantle {
    Window::~Window() { destroy(); }

    void Window::init(const Properties &properties, VirtualHeap *heap) {
        check(!m_is_initialized);

        m_logger = spdlog::get("window").get();
        m_tlsf_alloc.init(heap->take(kilobytes(128)));
        m_glfw_alloc.init(&m_tlsf_alloc);

        glfwInitAllocator(m_glfw_alloc.glfw_alloc());

        glfwSetErrorCallback([](int error, const char *desc) {
            spdlog::get("window")->error("GLFW error {}: {}", error, desc);
        });
        fatal(!glfwInit(), "Failed to initialize GLFW");
        m_logger->info("GLFW initialized");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_native_window =
            glfwCreateWindow(static_cast<int>(properties.size.width),
                             static_cast<int>(properties.size.height),
                             properties.title.data(), nullptr, nullptr);

        fatal(!m_native_window, "Failed to create GLFW window");
        glfwSetInputMode(m_native_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSetWindowUserPointer(m_native_window, this);

        m_logger->info("Window created: {} ({}x{})", properties.title.data(),
                     properties.size.width, properties.size.height);
        m_is_initialized = true;
    }

    void Window::destroy() {
        if (m_is_initialized) {
            glfwDestroyWindow(m_native_window);
            m_native_window = nullptr;
            m_logger->info("GLFW window destroyed");
            glfwTerminate();
            m_logger->info("GLFW terminated");

            m_is_initialized = false;
        }
    }

    void Window::update() {
        check(m_is_initialized);

        std::swap(m_pressed_keys, m_pressed_keys_prev);
        for (usize i = 0; i < glfw_keys.size(); i++) {
            m_pressed_keys[i] = glfwGetKey(m_native_window, glfw_keys[i]) == GLFW_PRESS;
        }

        std::swap(m_pressed_mb, m_pressed_mb_prev);
        for (usize i = 0; i < glfw_mb.size(); i++) {
            m_pressed_mb[i] = glfwGetMouseButton(m_native_window, glfw_mb[i]) == GLFW_PRESS;
        }

        glfwPollEvents();
    }

    bool Window::should_close() const {
        check(m_is_initialized);
        return glfwWindowShouldClose(m_native_window);
    }

    bool Window::is_key_pressed(Key key) const {
        check(m_is_initialized);
        return m_pressed_keys[std::to_underlying(key)];
    }

    bool Window::is_key_just_pressed(Key key) const {
        check(m_is_initialized);
        usize i = std::to_underlying(key);
        return m_pressed_keys[i] && !m_pressed_keys_prev[i];
    }

    bool Window::is_key_just_released(Key key) const {
        check(m_is_initialized);
        usize i = std::to_underlying(key);
        return !m_pressed_keys[i] && m_pressed_keys_prev[i];
    }

    bool Window::is_mouse_button_pressed(MouseButton button) const {
        check(m_is_initialized);
        return m_pressed_mb[std::to_underlying(button)];
    }

    bool Window::is_mouse_button_just_pressed(MouseButton button) const {
        check(m_is_initialized);
        usize i = std::to_underlying(button);
        return m_pressed_mb[i] && !m_pressed_mb_prev[i];
    }

    bool Window::is_mouse_button_just_released(MouseButton button) const {
        check(m_is_initialized);
        usize i = std::to_underlying(button);
        return !m_pressed_mb[i] && m_pressed_mb_prev[i];
    }

    Window::MousePosition Window::get_mouse_position() const {
        check(m_is_initialized);
        f64 x;
        f64 y;
        glfwGetCursorPos(m_native_window, &x, &y);
        return {static_cast<f32>(x), static_cast<f32>(y)};
    }

    u32 Window::get_width() const { return get_size().width; }

    u32 Window::get_height() const { return get_size().height; }

    Window::Properties::Size Window::get_size() const {
        check(m_is_initialized);
        int width = 0;
        int height = 0;
        glfwGetWindowSize(m_native_window, &width, &height);
        return {static_cast<u32>(width), static_cast<u32>(height)};
    }

    Window::Properties::Size Window::get_framebuffer_size() const {
        check(m_is_initialized);
        int width, height;
        glfwGetFramebufferSize(m_native_window, &width, &height);
        return {static_cast<u32>(width), static_cast<u32>(height)};
    }

    GLFWwindow *Window::get_native_window() const {
        check(m_is_initialized);
        return m_native_window;
    }

    f64 Window::get_time() const {
        check(m_is_initialized);
        return glfwGetTime();
    }

    void Window::set_resize_callback(std::function<void(u32, u32)> callback) {
        m_resize_callback = std::move(callback);
        glfwSetFramebufferSizeCallback(
            m_native_window, [](GLFWwindow *w, int width, int height) {
                auto *window =
                    static_cast<Window *>(glfwGetWindowUserPointer(w));
                if (window->m_resize_callback) {
                    window->m_resize_callback(static_cast<u32>(width),
                                              static_cast<u32>(height));
                }
            });
    }
} // namespace mantle

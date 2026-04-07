#pragma once

#include <functional>
#include <string>
#include "GLFW/glfw3.h"
#include "core/types.h"

namespace mantle {
    class Window final {
      public:
        struct Properties {
            struct Size {
                u32 width;
                u32 height;
            };

            std::string title;
            Size size;
        };

      public:
        Window() = default;
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
        Window(Window &&) noexcept = delete;
        Window &operator=(Window &&) noexcept = delete;

        void init(const Properties &properties);
        void destroy();

        void on_update() const;
        bool should_close() const;


        u32 get_width() const;
        u32 get_height() const;
        Properties::Size get_size() const;
        Properties::Size get_framebuffer_size() const;
        GLFWwindow *get_native_window() const;
        f64 get_time() const;

        void set_resize_callback(std::function<void(u32, u32)> callback);

      private:
        bool m_is_initialized = false;
        GLFWwindow *m_native_window = nullptr;

        std::function<void(u32, u32)> m_resize_callback{};

        inline static u32 s_windows_count = 0;
    };
} // namespace mantle

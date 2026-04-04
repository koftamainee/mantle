#pragma once
#include <memory>

namespace mantle {

    class Window;

    class Renderer final {
    public:
        Renderer();
        ~Renderer();

        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;
        Renderer(Renderer &&) noexcept = delete;
        Renderer &operator=(Renderer &&) noexcept = delete;

        void init(const Window &window);
        void destroy();

    private:
        bool m_is_initialized = false;

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };

} // namespace VkEngine

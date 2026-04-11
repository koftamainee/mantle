#pragma once

#include <GLFW/glfw3.h>

#include "core/memory/tlsf_allocator.h"

namespace mantle {

    class GlfwAllocator final {
    public:
        GlfwAllocator() = default;
        ~GlfwAllocator() = default;

        GlfwAllocator(const GlfwAllocator &) = delete;
        GlfwAllocator &operator=(const GlfwAllocator &) = delete;
        GlfwAllocator(GlfwAllocator &&) = delete;
        GlfwAllocator &operator=(GlfwAllocator &&) = delete;

        void init(TlsfAllocator *tlsf);

        const GLFWallocator *glfw_allocator() const;

    private:
        static void *glfw_alloc(usize size, void *user);
        static void *glfw_realloc(void *block, usize size, void *user);
        static void glfw_free(void *block, void *user);

    private:
        TlsfAllocator *m_tlsf = nullptr;
        GLFWallocator m_allocator = {};
    };

} // namespace mantle
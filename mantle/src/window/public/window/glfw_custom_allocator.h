#pragma once

#include <GLFW/glfw3.h>

#include "core/memory/tlsf_allocator.h"

namespace mantle {

    class GLFWCustomAllocator final {
      public:
        GLFWCustomAllocator() = default;
        ~GLFWCustomAllocator() = default;

        MANTLE_NO_COPY(GLFWCustomAllocator);

        void init(TlsfAllocator *tlsf);

        const GLFWallocator *glfw_alloc() const;

      private:
        static void *glfw_alloc(usize size, void *user);
        static void *glfw_realloc(void *block, usize size, void *user);
        static void glfw_free(void *block, void *user);

      private:
        TlsfAllocator *m_tlsf = nullptr;
        GLFWallocator m_allocator = {};
    };

} // namespace mantle

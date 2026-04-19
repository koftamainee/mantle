#include "../public/window/glfw_custom_allocator.h"
#include "core/assert.h"

namespace mantle {

    void GLFWCustomAllocator::init(TlsfAllocator *tlsf) {
        check(tlsf != nullptr);

        m_tlsf = tlsf;
        m_allocator.allocate = glfw_alloc;
        m_allocator.reallocate = glfw_realloc;
        m_allocator.deallocate = glfw_free;
        m_allocator.user = this;
    }

    const GLFWallocator *GLFWCustomAllocator::glfw_alloc() const {
        return &m_allocator;
    }

    void *GLFWCustomAllocator::glfw_alloc(usize size, void *user) {
        auto *self = static_cast<GLFWCustomAllocator *>(user);
        return self->m_tlsf->alloc(size);
    }

    void *GLFWCustomAllocator::glfw_realloc(void *block, usize size, void *user) {
        auto *self = static_cast<GLFWCustomAllocator *>(user);
        return self->m_tlsf->realloc(block, size);
    }

    void GLFWCustomAllocator::glfw_free(void *block, void *user) {
        auto *self = static_cast<GLFWCustomAllocator *>(user);
        self->m_tlsf->free(block);
    }

} // namespace mantle
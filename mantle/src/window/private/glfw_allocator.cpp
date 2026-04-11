#include "../public/window/glfw_allocator.h"
#include "core/assert.h"

namespace mantle {

    void GlfwAllocator::init(TlsfAllocator *tlsf) {
        check(tlsf != nullptr);

        m_tlsf = tlsf;
        m_allocator.allocate = glfw_alloc;
        m_allocator.reallocate = glfw_realloc;
        m_allocator.deallocate = glfw_free;
        m_allocator.user = this;
    }

    const GLFWallocator *GlfwAllocator::glfw_allocator() const {
        return &m_allocator;
    }

    void *GlfwAllocator::glfw_alloc(usize size, void *user) {
        auto *self = static_cast<GlfwAllocator *>(user);
        return self->m_tlsf->alloc(size);
    }

    void *GlfwAllocator::glfw_realloc(void *block, usize size, void *user) {
        auto *self = static_cast<GlfwAllocator *>(user);
        return self->m_tlsf->realloc(block, size);
    }

    void GlfwAllocator::glfw_free(void *block, void *user) {
        auto *self = static_cast<GlfwAllocator *>(user);
        self->m_tlsf->free(block);
    }

} // namespace mantle
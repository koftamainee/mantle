#include "core/memory/tlsf_allocator.h"
#include "core/assert.h"

#include <tlsf.h>

namespace mantle {

    TlsfAllocator::~TlsfAllocator() { destroy(); }

    void TlsfAllocator::init(MemoryBlock block) {
        check(!m_is_initialized);
        check(block.ptr != nullptr);
        check(block.size > 0);

        m_tlsf = tlsf_create_with_pool(block.ptr, block.size);
        fatal(m_tlsf == nullptr, "Out of memory");
        m_is_initialized = true;
    }

    void TlsfAllocator::destroy() {
        if (m_is_initialized) {
            tlsf_destroy(m_tlsf);
            m_tlsf = nullptr;
            m_is_initialized = false;
        }
    }

    void *TlsfAllocator::alloc(usize size) {
        check(m_is_initialized);
        check(size > 0);

        void *ptr = tlsf_malloc(m_tlsf, size);
        fatal(ptr == nullptr, "Out of memory");
        return ptr;
    }

    void *TlsfAllocator::alloc_aligned(usize size, usize align) {
        check(m_is_initialized);
        check(size > 0);
        check(align > 0);

        void *ptr = tlsf_memalign(m_tlsf, align, size);
        fatal(ptr == nullptr, "Out of memory");
        return ptr;
    }

    void *TlsfAllocator::realloc(void *ptr, usize size) {
        check(m_is_initialized);
        check(size > 0);

        void *result = tlsf_realloc(m_tlsf, ptr, size);
        fatal(result == nullptr, "Out of memory");
        return result;
    }

    void TlsfAllocator::free(void *ptr) {
        check(m_is_initialized);
        check(ptr != nullptr);

        tlsf_free(m_tlsf, ptr);
    }

} // namespace mantle

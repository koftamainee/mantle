#pragma once

#include "core/memory/memory_block.h"
#include "core/types.h"
#include "tlsf.h"

namespace mantle {

    class TlsfAllocator final {
    public:
        TlsfAllocator() = default;
        ~TlsfAllocator();

        TlsfAllocator(const TlsfAllocator &) = delete;
        TlsfAllocator &operator=(const TlsfAllocator &) = delete;
        TlsfAllocator(TlsfAllocator &&) = delete;
        TlsfAllocator &operator=(TlsfAllocator &&) = delete;

        void init(MemoryBlock block);
        void destroy();

        [[nodiscard]] void *alloc(usize size);
        [[nodiscard]] void *alloc_aligned(usize size, usize align);
        [[nodiscard]] void *realloc(void *ptr, usize size);
        void free(void *ptr);

    private:
        tlsf_t m_tlsf = nullptr;
        bool m_is_initialized = false;
    };

} // namespace mantle
#pragma once

#include <memory_resource>

#include "virtual_heap.h"

namespace mantle {
    class PersistentResource : public std::pmr::memory_resource {
    public:
        PersistentResource() : m_heap(nullptr) {}
        explicit PersistentResource(VirtualHeap *heap) : m_heap(heap) {}

    private:
        void *do_allocate(usize size, usize align) override {
            check(m_heap != nullptr);
            return m_heap->take(size, align);
        }

        void do_deallocate(void *memory, usize size, usize align) override {}

        bool do_is_equal(const memory_resource &other) const noexcept override {
            return this == &other;
        }

    private:
        VirtualHeap *m_heap;
    };
} // namespace mantle

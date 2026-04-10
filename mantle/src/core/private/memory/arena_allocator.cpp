#include "core/memory/arena_allocator.h"
#include "core/assert.h"

namespace mantle {

    ArenaAllocator::~ArenaAllocator() { destroy(); }

    void ArenaAllocator::init(VirtualHeap &heap, usize size) {
        check(!m_is_initialized);
        check(size > 0);

        m_base = heap.take(size);
        m_size = size;
        m_offset = 0;
        m_is_initialized = true;
    }

    void ArenaAllocator::destroy() {
        if (m_is_initialized) {
            m_base = nullptr;
            m_size = 0;
            m_offset = 0;
            m_is_initialized = false;
        }
    }

    void *ArenaAllocator::push(usize size, usize align) {
        check(m_is_initialized);

        usize aligned_offset = (m_offset + (align - 1)) & ~(align - 1);
        usize new_offset = aligned_offset + size;

        check(new_offset <= m_size);

        void *ptr = static_cast<u8 *>(m_base) + aligned_offset;
        m_offset = new_offset;
        return ptr;
    }

    ArenaAllocator::Marker ArenaAllocator::save() const {
        check(m_is_initialized);
        return {m_offset};
    }

    void ArenaAllocator::restore(Marker marker) {
        check(m_is_initialized);
        check(marker.offset <= m_offset);
        m_offset = marker.offset;
    }

    void ArenaAllocator::reset() {
        check(m_is_initialized);
        m_offset = 0;
    }

    usize ArenaAllocator::size() const {
        check(m_is_initialized);
        return m_size;
    }

    usize ArenaAllocator::offset() const {
        check(m_is_initialized);
        return m_offset;
    }

} // namespace mantle

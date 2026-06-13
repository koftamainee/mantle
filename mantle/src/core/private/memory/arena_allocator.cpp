// Copyright 2026 Mantle
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "mantle/core/memory/arena_allocator.h"

#include "mantle/core/assert.h"

namespace mantle {

    ArenaAllocator::~ArenaAllocator() { destroy(); }

    void ArenaAllocator::init(MemoryBlock block, std::string_view debug_name) {
        MANTLE_CHECK(!m_is_initialized);
        MANTLE_CHECK(block.ptr != nullptr);
        MANTLE_CHECK(block.size > 0);

        m_base = block.ptr;
        m_size = block.size;
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
        MANTLE_CHECK(m_is_initialized);

        usize aligned_offset = (m_offset + (align - 1)) & ~(align - 1);
        usize new_offset = aligned_offset + size;

        MANTLE_FATAL(new_offset > m_size, "Out of memory");

        void *ptr = static_cast<u8 *>(m_base) + aligned_offset;
        m_offset = new_offset;
        return ptr;
    }

    void ArenaAllocator::free(void *ptr) {
        usize new_offset = static_cast<char *>(ptr) - static_cast<char *>(m_base);
        MANTLE_CHECK(new_offset < m_size);

        m_offset = new_offset;
    }

    ArenaAllocator::Marker ArenaAllocator::save() const {
        MANTLE_CHECK(m_is_initialized);
        return {m_offset};
    }

    void ArenaAllocator::restore(Marker marker) {
        MANTLE_CHECK(m_is_initialized);
        MANTLE_CHECK(marker.offset <= m_offset);
        m_offset = marker.offset;
    }

    void ArenaAllocator::reset() {
        MANTLE_CHECK(m_is_initialized);
        m_offset = 0;
    }

    usize ArenaAllocator::size() const {
        MANTLE_CHECK(m_is_initialized);
        return m_size;
    }

    usize ArenaAllocator::offset() const {
        MANTLE_CHECK(m_is_initialized);
        return m_offset;
    }

    usize ArenaAllocator::remaining() const {
        MANTLE_CHECK(m_is_initialized);
        return m_size - m_offset;
    }

} // namespace mantle

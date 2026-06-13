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

#include "arena_temp_allocator.h"

namespace mantle {
    void ArenaTempAllocator::init(MemoryBlock mem, std::string_view debug_name) {
        MANTLE_CHECK(!m_is_initialized);
        m_allocator.init(mem, debug_name);
        m_is_initialized = true;
    }

    void ArenaTempAllocator::destroy() {
        if (m_is_initialized) {
            m_allocator.destroy();
            m_is_initialized = false;
        }
    }

    void *ArenaTempAllocator::Allocate(JPH::uint inSize) {
        MANTLE_CHECK(m_is_initialized);
        return m_allocator.alloc(inSize);
    }

    void ArenaTempAllocator::Free(void *inAddress, JPH::uint inSize) {
        MANTLE_CHECK(m_is_initialized);
        m_allocator.free(inAddress);
    }

    void ArenaTempAllocator::Reset() { m_allocator.reset(); }
} // namespace mantle

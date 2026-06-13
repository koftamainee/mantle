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

#pragma once

#include <memory_resource>

#include "mantle/core/assert.h"
#include "mantle/core/memory/tlsf_allocator.h"

namespace mantle {

    class TlsfResource final : public std::pmr::memory_resource {
      public:
        TlsfResource() : m_allocator(nullptr) {}
        explicit TlsfResource(TlsfAllocator *allocator) : m_allocator(allocator) {}

      private:
        void *do_allocate(usize size, usize align) override {
            MANTLE_CHECK(m_allocator != nullptr);
            return m_allocator->alloc(size, align);
        }

        void do_deallocate(void *memory, usize size, usize align) override {
            MANTLE_CHECK(m_allocator != nullptr);
            m_allocator->free(memory);
        }

        bool do_is_equal(const memory_resource &other) const noexcept override {
            return this == &other;
        }

      private:
        TlsfAllocator *m_allocator;
    };

} // namespace mantle

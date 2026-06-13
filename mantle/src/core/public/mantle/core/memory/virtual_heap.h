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

#include "mantle/core/macros.h"
#include "mantle/core/memory/memory_block.h"
#include "mantle/core/memory/os_memory.h"
#include "mantle/core/types.h"

namespace mantle {

    class VirtualHeap final {
      public:
        VirtualHeap() = default;
        ~VirtualHeap();

        MANTLE_NO_COPY(VirtualHeap);

        void init(OSMemory &os, usize reserve_size);
        void destroy();

        [[nodiscard]] MemoryBlock take(usize size);

        usize reserved() const;
        usize used() const;
        usize committed() const;

      private:
        OSMemory *m_os = nullptr;
        void     *m_base = nullptr;
        usize     m_reserved = 0;
        usize     m_used = 0;
        usize     m_committed = 0;
        bool      m_is_initialized = false;
    };

} // namespace mantle

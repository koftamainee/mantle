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

// fix-includes off
#include <Jolt/Jolt.h>
// fix-includes on

#include <Jolt/Core/TempAllocator.h>

#include "mantle/core/macros.h"
#include "mantle/core/memory/arena_allocator.h"
#include "mantle/core/memory/memory_block.h"
#include "mantle/core/memory/thread_safe_allocator.h"

namespace mantle {
    class ArenaTempAllocator final : public JPH::TempAllocator {
      public:
        MANTLE_DEFAULT_INIT(ArenaTempAllocator);

        void init(MemoryBlock mem, std::string_view debug_name = {});
        void destroy();

        void *Allocate(JPH::uint inSize) override;
        void  Free(void *inAddress, JPH::uint inSize) override;
        void  Reset();

      private:
        bool                                m_is_initialized = false;
        ThreadSafeAllocator<ArenaAllocator> m_allocator {};
    };
} // namespace mantle

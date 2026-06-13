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

#include "arena_allocator.h"

namespace mantle {
    struct ScopeArena final {
        ArenaAllocator        *arena;
        ArenaAllocator::Marker tag;

        // NOTE: maybe use only no copy, and implement move operator
        MANTLE_NO_COPY_NO_MOVE(ScopeArena);

        explicit ScopeArena(ArenaAllocator *a) : arena(a), tag(a->save()) {}
        ~ScopeArena() { arena->restore(tag); }
    };
} // namespace mantle

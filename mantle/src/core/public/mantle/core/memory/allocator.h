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

#include <concepts>
#include <string_view>

#include "mantle/core/memory/memory_block.h"
#include "mantle/core/types.h"

namespace mantle {

    template <typename A>
    concept CAllocator = (!std::copyable<A>) && requires(A a, void *ptr, usize size, usize align,
                                                         MemoryBlock block, std::string_view name) {
        { a.init(block, name) } -> std::same_as<void>;
        { a.destroy() } -> std::same_as<void>;

        { a.alloc(size, align) } -> std::same_as<void *>;
        { a.alloc(size) } -> std::same_as<void *>;
        { a.realloc(ptr, size) } -> std::same_as<void *>;
        { a.free(ptr) } -> std::same_as<void>;
    };

    template <typename A>
    concept CResetableAllocator = requires(A a) {
        { a.reset() } -> std::same_as<void>;
    };

    template <typename A, typename T>
    concept CTypedAllocator = CAllocator<A> && requires(A a, usize count) {
        { a.template alloc<T>() } -> std::same_as<T *>;
        { a.template alloc<T>(count) } -> std::same_as<T *>;
        { a.template alloc_zeroed<T>() } -> std::same_as<T *>;
        { a.template alloc_zeroed<T>(count) } -> std::same_as<T *>;
    };

    template <typename A, typename T, typename... Args>
    concept CEmplaceAllocator =
        CAllocator<A> && std::constructible_from<T, Args...> && requires(A a, Args &&...args) {
            { a.template emplace<T>(std::forward<Args>(args)...) } -> std::same_as<T *>;
        };

} // namespace mantle

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

#include <array>

#include "mantle/core/assert.h"
#include "mantle/core/types.h"

namespace mantle {

    struct MemoryBlock final {
        void *ptr;
        usize size;

        template <usize... Sizes>
        constexpr std::array<MemoryBlock, sizeof...(Sizes)> split() const {
            constexpr usize count = sizeof...(Sizes);
            constexpr usize total = (Sizes + ...);

            std::array<MemoryBlock, count> result {};

            MANTLE_CHECK(ptr != nullptr && size >= total);
            for (auto &block : result) {
                block.ptr = nullptr;
                block.size = 0;
            }

            auto *base = static_cast<char *>(ptr);
            usize offset = 0;
            usize idx = 0;
            ((result[idx] = MemoryBlock {base + offset, Sizes}, offset += Sizes, ++idx), ...);
            return result;
        }

        template <usize... Percentages>
        constexpr std::array<MemoryBlock, sizeof...(Percentages)> split_ratio() const {
            constexpr usize count = sizeof...(Percentages);
            constexpr usize total = (Percentages + ...);

            static_assert(count > 0);
            static_assert(total > 0);

            std::array<MemoryBlock, count> result {};
            std::array<usize, count>       percentages {Percentages...};
            std::array<usize, count>       sizes {};

            MANTLE_CHECK(ptr != nullptr);
            MANTLE_CHECK(size > 0);

            usize allocated = 0;

            for (usize i = 0; i < count; ++i) {
                sizes[i] = (size * percentages[i]) / total;
                allocated += sizes[i];
            }

            usize remainder = size - allocated;

            for (usize i = 0; i < count && remainder > 0; ++i) {
                ++sizes[i];
                --remainder;
            }

            auto *base = static_cast<std::byte *>(ptr);

            usize offset = 0;

            for (usize i = 0; i < count; ++i) {
                result[i] = MemoryBlock {
                    .ptr = base + offset,
                    .size = sizes[i],
                };

                offset += sizes[i];
            }

            return result;
        }
    };

} // namespace mantle

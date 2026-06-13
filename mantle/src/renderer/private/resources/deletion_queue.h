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


#include <ranges>
#include <vector>

#include "mantle/core/memory/arena_allocator.h"
#include "mantle/core/memory/memory_block.h"
#include "mantle/core/memory/memory_units.h"
#include "mantle/core/memory/pmr/arena_resource.h"

namespace mantle {
    template <typename F>
    concept CDeletable = requires(F f) {
        { f() } -> std::same_as<void>;
    };

    class DeletionQueue {
      public:
        DeletionQueue() = default;
        ~DeletionQueue() { flush(); }

        MANTLE_NO_COPY_NO_MOVE(DeletionQueue);

        void init(MemoryBlock block) {
            m_arena.init(block);
            m_resource = ArenaResource(&m_arena);
            m_deletors = std::pmr::vector<Deletor>(&m_resource);
        }

        template <typename T>
        void push(T *ptr) {
            m_deletors.push_back({[](void *p) { static_cast<T *>(p)->~T(); }, ptr});
        }

        template <typename F>
            requires CDeletable<F>
        void push_fn(F &&fn) {
            using Fn = std::decay_t<F>;

            Fn *f = m_arena.emplace<Fn>(std::forward<F>(fn));

            m_deletors.push_back({[](void *p) {
                                      auto func = static_cast<Fn *>(p);
                                      (*func)();
                                      func->~Fn();
                                  },
                                  f});
        }

        void flush() {
            for (auto &m_deletor : std::views::reverse(m_deletors)) {
                m_deletor.call(m_deletor.data);
            }
            m_deletors.clear();
            m_deletors.shrink_to_fit();
            m_arena.reset();
        }

      private:
        struct Deletor {
            void (*call)(void *);
            void *data;
        };

        std::pmr::vector<Deletor> m_deletors;
        ArenaAllocator            m_arena;
        ArenaResource             m_resource;
    };
} // namespace mantle

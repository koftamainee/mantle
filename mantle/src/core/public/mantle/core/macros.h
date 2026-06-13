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
#include "types.h"

#if defined(__GNUC__) || defined(__clang__)
#define MANTLE_UNLIKELY(x) (__builtin_expect(!!(x), 0))
#define MANTLE_LIKELY(x) (__builtin_expect(!!(x), 1))
#else
#define MANTLE_UNLIKELY(x) (x)
#define MANTLE_LIKELY(x) (x)
#endif

#define MANTLE_NO_COPY(type)                                                                       \
    type(const type &) = delete;                                                                   \
    type &operator=(const type &) = delete

#define MANTLE_NO_MOVE(type)                                                                       \
    type(type &&) noexcept = delete;                                                               \
    type &operator=(type &&) noexcept = delete

#define MANTLE_NO_COPY_NO_MOVE(type)                                                               \
    MANTLE_NO_COPY(type);                                                                          \
    MANTLE_NO_MOVE(type)

#define MANTLE_DEFAULT_INIT(type)                                                                  \
    type() = default;                                                                              \
    ~type() {                                                                                      \
        if (m_is_initialized) {                                                                    \
            destroy();                                                                             \
        }                                                                                          \
    }                                                                                              \
    MANTLE_NO_COPY_NO_MOVE(type)

#define MANTLE_TODO() MANTLE_FATAL(true, "Implement me")

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

#include "mantle/core/types.h"

namespace mantle {

    constexpr usize bytes(usize n) { return n; }
    constexpr usize kilobytes(usize n) { return n * 1024; }
    constexpr usize megabytes(usize n) { return n * 1024 * 1024; }
    constexpr usize gigabytes(usize n) { return n * 1024ull * 1024ull * 1024ull; }

} // namespace mantle

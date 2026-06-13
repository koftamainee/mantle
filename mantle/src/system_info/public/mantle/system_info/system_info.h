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

#include <spdlog/logger.h>

#include <cstdint>
#include <string_view>

#include "mantle/core/types.h"

namespace mantle {
    void log_system_info(spdlog::logger *logger, std::string_view gpu_name = {},
                         uint64_t vram_bytes = 0, u32 window_w = 0, u32 window_h = 0,
                         bool vsync = false, f32 refresh_rate = 0.0f, bool fullscreen = false);
}

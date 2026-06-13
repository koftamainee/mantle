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

#include "mantle/noise/sampler.h"

namespace mantle::detail {
    glm::vec2 seed_offset(u32 seed) {
        return glm::vec2 {
            static_cast<f32>(seed) * 31.7f,
            static_cast<f32>(seed) * 73.1f,
        };
    }

    glm::vec3 seed_offset_3d(u32 seed) {
        return glm::vec3 {
            static_cast<f32>(seed) * 31.7f,
            static_cast<f32>(seed) * 73.1f,
            static_cast<f32>(seed) * 97.3f,
        };
    }
} // namespace mantle::detail

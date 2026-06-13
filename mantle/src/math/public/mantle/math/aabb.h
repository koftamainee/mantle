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

#include <glm/vec3.hpp>

namespace mantle {
    struct AABB final {
        glm::vec3 min;
        glm::vec3 max;

        bool intersects(const AABB &other) const;
        bool contains(glm::vec3 point) const;

        glm::vec3 size() const;
        glm::vec3 center() const;

        void expand(glm::vec3 point);
    };
} // namespace mantle

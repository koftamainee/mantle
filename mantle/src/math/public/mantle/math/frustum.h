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

#include <glm/glm.hpp>

#include <array>

#include "aabb.h"
#include "plane.h"

namespace mantle {
    struct Frustum final {

        void extract(const glm::mat4 &vp);
        bool intersects(const AABB &aabb) const;

      private:
        enum class Side {
            Left,
            Right,
            Bottom,
            Top,
            Near,
            Far,
        };
        std::array<Plane, 6> m_planes {};
    };
} // namespace mantle

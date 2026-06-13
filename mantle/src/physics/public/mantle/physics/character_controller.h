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

#include "mantle/core/macros.h"
#include "mantle/core/types.h"

namespace mantle {

    class PhysicsSystem;

    class CharacterController final {
      public:
        MANTLE_DEFAULT_INIT(CharacterController);

        void init(PhysicsSystem &physics, glm::vec3 start_pos);
        void destroy();

        void      move(glm::vec3 velocity, f32 dt);
        glm::vec3 get_position() const;
        bool      is_grounded() const;

        glm::vec3 gravity = {0, -9.81f, 0};

      private:
        bool m_is_initialized = false;

        struct Impl;
        Impl *m_impl = nullptr;
    };

} // namespace mantle

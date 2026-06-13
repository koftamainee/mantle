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

// fix-includes off
#include <Jolt/Jolt.h>
// fix-includes on

#include <Jolt/Physics/Character/CharacterVirtual.h>

#include "layers.h"
#include "mantle/physics/character_controller.h"
#include "mantle/physics/physics_system.h"

namespace mantle {

    struct CharacterController::Impl final {
        JPH::Ref<JPH::CharacterVirtual> character;
        PhysicsSystem                  *physics_system = nullptr;

        constexpr static f32 kCapsuleRadius = 0.3f;
        constexpr static f32 kCapsuleHalfHeight = 0.5f;
        constexpr static f32 kMaxSlopeAngle = JPH::DegreesToRadians(45.0f);

        explicit Impl(PhysicsSystem &physics, glm::vec3 start_pos);
        ~Impl() = default;
    };

} // namespace mantle

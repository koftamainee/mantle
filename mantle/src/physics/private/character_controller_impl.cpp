// Copyright (c) 2026 Mantle. All rights reserved.
#include "character_controller_impl.h"

// fix-includes off
#include <Jolt/Jolt.h>
// fix-includes on

#include "physics_system_impl.h"


#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

namespace mantle {

    CharacterController::Impl::Impl(PhysicsSystem &physics, glm::vec3 start_pos) {

    }


    void CharacterController::init(PhysicsSystem &physics, glm::vec3 start_pos) {
        MANTLE_CHECK(!m_is_initialized);
        m_impl = new Impl(physics, start_pos); // fuck we use global heap
        m_is_initialized = true;
    }

    void CharacterController::destroy() {
        if (m_is_initialized) {
            delete m_impl;
            m_impl = nullptr;
            m_is_initialized = false;
        }
    }

    void CharacterController::move(glm::vec3 velocity, f32 dt) {
        MANTLE_CHECK(m_is_initialized);
    }

    glm::vec3 CharacterController::get_position() const {
        MANTLE_CHECK(m_is_initialized);

    }

    bool CharacterController::is_grounded() const {

    }

} // namespace mantle
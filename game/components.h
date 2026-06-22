#pragma once

#include <glm/glm.hpp>

#include "mantle/core/types.h"

namespace game {
    using mantle::f32;

    struct FlyCamera {
        f32 yaw   = 0.0f;
        f32 pitch = 0.0f;
        f32 speed = 500.0f;
        f32 mouse_sensitivity = 1.0f;
        glm::vec3 position {0.0f, 200.0f, 800.0f};
    };
} // namespace game

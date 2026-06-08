// Copyright (c) 2026 Mantle. All rights reserved.

#pragma once

#include <glm/glm.hpp>

#include "core/types.h"

namespace mantle {
    struct Camera {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 front = {0.0f, 0.0f, -1.0f};
        glm::vec3 right = {1.0f, 0.0f, 0.0f};
        f32       yaw = 0.0f;
        f32       pitch = 0.0f;
        f32       aspect = 1.0f;
        f32       fov = 75.0f;
        glm::mat4 view_proj = glm::mat4(1.0f);
    };
} // namespace mantle

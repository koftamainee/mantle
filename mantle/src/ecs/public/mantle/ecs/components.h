// Copyright (c) 2026 Mantle. All rights reserved.

#pragma once

#include <glm/glm.hpp>

#include "mantle/core/types.h"

namespace mantle {
    struct Transform {
        glm::vec3 position {0.0f};
        glm::vec3 rotation {0.0f};
        glm::vec3 scale {1.0f};
    };

} // namespace mantle

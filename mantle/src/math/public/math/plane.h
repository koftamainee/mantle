#pragma once
#include "core/types.h"
#include "glm/glm.hpp"

namespace mantle {
    struct Plane final {
        glm::vec3 normal;
        f32 distance;

        f32 signed_distance(glm::vec3 point) const;
    };
}
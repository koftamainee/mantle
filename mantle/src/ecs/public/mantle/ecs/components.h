#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "mantle/assets/types.h"
#include "mantle/core/types.h"

namespace mantle {

    struct LocalTransform {
        glm::vec3 translation {0.0f};
        glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 scale {1.0f};
    };

    struct MeshRenderer {
        MeshHandle      mesh {};
        MaterialHandle materials[10]; // TODO: fix me later please
    };

} // namespace mantle

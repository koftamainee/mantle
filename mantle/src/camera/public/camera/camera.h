#pragma once
#include <glm/glm.hpp>

namespace mantle {
    struct Camera final {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 front = {0.0f, 0.0f, -1.0f};
        glm::vec3 up = {0.0f, 1.0f, 0.0f};

        float fov = 75.0f;
        float aspect = 16.0f / 9.0f;
        float near = 0.1f;
        float far = 1000.0f;

        glm::mat4 view() const;
        glm::mat4 projection() const;
    };
}

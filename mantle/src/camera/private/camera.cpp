#include <camera/camera.h>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace mantle {
    glm::mat4 Camera::view() const {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 Camera::projection() const {
return glm::perspective(glm::radians(fov), aspect, near, far);
    }
}

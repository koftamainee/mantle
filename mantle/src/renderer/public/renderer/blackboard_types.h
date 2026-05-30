#pragma once
#include <glm/glm.hpp>

#include "renderer/types.h"

namespace mantle {
    struct BbBackbuffer      { FGImageHandle handle; };
    struct BbCameraData      { glm::mat4 view_proj; };
    struct BbFramebufferSize { u32 width; u32 height; };
} // namespace mantle

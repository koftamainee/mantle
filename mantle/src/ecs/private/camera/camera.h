#pragma once

#include "flecs.h"
#include "core/types.h"

namespace mantle {
    void bootstrap_camera(const flecs::world &world, f32 aspect);
}
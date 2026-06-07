#pragma once

#include <flecs.h>

namespace mantle {
    class Window;
    void bootstrap_rumbling(const flecs::world &world, Window &window);
}

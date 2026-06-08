// Copyright (c) 2026 Mantle. All rights reserved.

#pragma once

#include <flecs.h>

namespace mantle {
    class Window;
    void bootstrap_rumbling(const flecs::world &world, Window &window);
} // namespace mantle

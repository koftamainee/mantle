// Copyright (c) 2026 Mantle. All rights reserved.

#include "rumbling.h"

#include "input/components.h"
#include "window/window.h"

namespace mantle {
    void bootstrap_rumbling(const flecs::world &world, Window &window) {
        (void)world.component<InputState>();

        world.system<>("Rumble").kind(flecs::PreUpdate).run([&window](const flecs::iter &it) {
            auto        ecs_world = it.world();
            const auto &player_input = ecs_world.get<InputState>();

            if (player_input.sprint) {
                if (player_input.controller_active) {
                    window.set_controller_rumble(0x6000, 0x1000, UINT32_MAX);
                }
            } else {
                window.stop_controller_rumble();
            }
        });
    }
} // namespace mantle

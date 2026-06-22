#include "actions.h"
#include "assets.gen.h"
#include "camera_system.h"
#include "components.h"
#include "flecs.h"
#include "mantle/ecs/components.h"
#include "mantle/engine/engine.h"
#include "mantle/window/window.h"

using namespace mantle;
using namespace game;


void register_inputs(InputSystem &input) {
    input.bind_axis(GameAction::MoveForward, Key::S, Key::W);
    input.bind_axis(GameAction::StrafeRight, Key::A, Key::D);
    input.bind_axis(GameAction::MoveUp, Key::LShift, Key::Space);
    input.bind_axis(GameAction::LookX, MouseAxis::MoveX, 50.0f);
    input.bind_axis(GameAction::LookY, MouseAxis::MoveY, 50.0f);
}

int main() {
    EngineConfig cfg;
    cfg.window = {.title = "Mantle Engine", .size = {0, 0}, .fullscreen = false};

    Engine engine;
    engine.init(cfg);

    register_inputs(engine.input_system());

    flecs::world &world = engine.world();
    auto &assets = engine.assets();

    world.entity("FlyCamera")
        .set(FlyCamera {});

    register_fly_camera_system(world);

    SceneHandle model = assets.preload(scenes::models::kJustAGirl);

    assets.instantiate(world, model);

    engine.run();

    assets.unload(model);
    engine.destroy();


    return 0;
}

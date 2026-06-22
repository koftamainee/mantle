#pragma once

#include "mantle/input/input_system.h"

namespace game {
    MANTLE_ACTIONS(GameAction)
    MoveForward, StrafeRight, MoveUp,
    LookX, LookY,
    MANTLE_ACTIONS_END()

}

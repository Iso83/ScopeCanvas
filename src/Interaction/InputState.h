#pragma once

#include "Engine/DiagramModel.h"

namespace Interaction {

struct InputState {
    Engine::Vec2 worldMousePosition {0.0F, 0.0F};
    bool leftPressed {false};
    bool leftReleased {false};
    bool leftDown {false};
};

} // namespace Interaction

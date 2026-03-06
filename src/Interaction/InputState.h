#pragma once

struct InputState {
    double mouseX = 0.0;
    double mouseY = 0.0;

    bool leftDown = false;
    bool middleDown = false;

    float scrollDelta = 0.0f;
};

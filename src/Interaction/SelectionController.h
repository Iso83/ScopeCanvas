#pragma once

#include "Engine/DiagramModel.h"

#include <glm/vec2.hpp>

class SelectionController {
public:
    void onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld);

private:
    static Node* hitTest(DiagramModel& model, const glm::vec2& mouseWorld);
};

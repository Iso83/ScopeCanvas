#pragma once

#include "Engine/DiagramModel.h"

#include <glm/vec2.hpp>

class DragController {
public:
    void onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld);
    void onMouseUp();
    void update(const glm::vec2& mouseWorld);

private:
    static Node* hitTestSelectedNode(DiagramModel& model, const glm::vec2& mouseWorld);

    bool m_dragging = false;
    Node* m_draggedNode = nullptr;
    glm::vec2 m_dragOffset = glm::vec2(0.0f);
};

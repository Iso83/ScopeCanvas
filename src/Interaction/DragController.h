#pragma once

#include "Engine/DiagramModel.h"

#include <glm/vec2.hpp>

class DragController {
public:
    void onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld);
    void onMouseUp();
    void update(DiagramModel& model, const glm::vec2& mouseWorld);
    bool isDragging() const { return m_dragging; }

private:
    static Node* hitTestSelectedNode(DiagramModel& model, const glm::vec2& mouseWorld);

    bool m_dragging = false;
    uint32_t m_draggedNodeId = 0;
    glm::vec2 m_dragOffset = glm::vec2(0.0f);
};

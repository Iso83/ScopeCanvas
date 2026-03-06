#include "Interaction/DragController.h"

void DragController::onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld) {
    m_draggedNode = hitTestSelectedNode(model, mouseWorld);
    if (m_draggedNode == nullptr) {
        m_dragging = false;
        return;
    }

    m_dragging = true;
    m_dragOffset = mouseWorld - m_draggedNode->position;
}

void DragController::onMouseUp() {
    m_dragging = false;
    m_draggedNode = nullptr;
}

void DragController::update(const glm::vec2& mouseWorld) {
    if (!m_dragging || m_draggedNode == nullptr) {
        return;
    }

    m_draggedNode->position = mouseWorld - m_dragOffset;
}

Node* DragController::hitTestSelectedNode(DiagramModel& model, const glm::vec2& mouseWorld) {
    for (auto it = model.nodes().rbegin(); it != model.nodes().rend(); ++it) {
        Node& node = *it;
        if (!node.selected) {
            continue;
        }

        const float minX = node.position.x;
        const float maxX = node.position.x + node.size.x;
        const float minY = node.position.y;
        const float maxY = node.position.y + node.size.y;

        if (mouseWorld.x >= minX && mouseWorld.x <= maxX &&
            mouseWorld.y >= minY && mouseWorld.y <= maxY) {
            return &node;
        }
    }

    return nullptr;
}

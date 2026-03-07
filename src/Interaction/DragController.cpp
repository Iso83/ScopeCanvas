#include "Interaction/DragController.h"

void DragController::onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld) {
    Node* draggedNode = hitTestSelectedNode(model, mouseWorld);
    if (draggedNode == nullptr) {
        m_dragging = false;
        m_draggedNodeId = 0;
        return;
    }

    m_dragging = true;
    m_draggedNodeId = draggedNode->id;
    m_dragOffset = mouseWorld - draggedNode->position;
}

void DragController::onMouseUp() {
    m_dragging = false;
    m_draggedNodeId = 0;
}

void DragController::update(DiagramModel& model, const glm::vec2& mouseWorld) {
    if (!m_dragging || m_draggedNodeId == 0) {
        return;
    }

    Node* draggedNode = model.findNode(m_draggedNodeId);
    if (draggedNode == nullptr) {
        m_dragging = false;
        m_draggedNodeId = 0;
        return;
    }

    draggedNode->position = mouseWorld - m_dragOffset;
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

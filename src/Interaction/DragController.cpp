#include "Interaction/DragController.h"

void DragController::onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld) {
    Node* anchorNode = hitTestSelectedNode(model, mouseWorld);
    if (anchorNode == nullptr) {
        m_dragging = false;
        m_anchorNodeId = 0;
        m_draggedNodes.clear();
        return;
    }

    m_dragging = true;
    m_anchorNodeId = anchorNode->id;
    m_anchorNodeStart = anchorNode->position;
    m_dragOffset = mouseWorld - anchorNode->position;

    m_draggedNodes.clear();
    for (const Node& node : model.nodes()) {
        if (node.selected) {
            m_draggedNodes.push_back({node.id, node.position});
        }
    }
}

void DragController::onMouseUp() {
    m_dragging = false;
    m_anchorNodeId = 0;
    m_draggedNodes.clear();
}

void DragController::update(DiagramModel& model, const glm::vec2& mouseWorld) {
    if (!m_dragging || m_anchorNodeId == 0) {
        return;
    }

    Node* anchorNode = model.findNode(m_anchorNodeId);
    if (anchorNode == nullptr) {
        m_dragging = false;
        m_anchorNodeId = 0;
        m_draggedNodes.clear();
        return;
    }

    const glm::vec2 anchorTargetPosition = mouseWorld - m_dragOffset;
    const glm::vec2 delta = anchorTargetPosition - m_anchorNodeStart;

    for (const DraggedNodeState& draggedNode : m_draggedNodes) {
        Node* node = model.findNode(draggedNode.nodeId);
        if (node == nullptr) {
            continue;
        }

        node->position = draggedNode.startPosition + delta;
    }
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

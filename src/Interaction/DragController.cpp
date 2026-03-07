#include "Interaction/DragController.h"

void DragController::onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld) {
    Node* hitNode = hitTestNode(model, mouseWorld);
    if (hitNode == nullptr || !hitNode->selected) {
        reset();
        return;
    }

    m_dragItems.clear();
    for (const Node& node : model.nodes()) {
        if (!node.selected) {
            continue;
        }

        m_dragItems.push_back({node.id, node.position});
    }

    if (m_dragItems.empty()) {
        reset();
        return;
    }

    m_dragging = true;
    m_dragStartWorld = mouseWorld;
}

std::vector<MoveNodesCommand::MoveItem> DragController::onMouseUp(DiagramModel& model) {
    std::vector<MoveNodesCommand::MoveItem> moveItems;
    if (!m_dragging) {
        return moveItems;
    }

    moveItems.reserve(m_dragItems.size());
    for (const DragItem& dragItem : m_dragItems) {
        const Node* node = model.findNode(dragItem.nodeId);
        if (node == nullptr) {
            continue;
        }

        if (node->position == dragItem.startPosition) {
            continue;
        }

        moveItems.push_back({dragItem.nodeId, dragItem.startPosition, node->position});
    }

    reset();
    return moveItems;
}

void DragController::update(DiagramModel& model, const glm::vec2& mouseWorld) {
    if (!m_dragging) {
        return;
    }

    const glm::vec2 delta = mouseWorld - m_dragStartWorld;
    for (const DragItem& dragItem : m_dragItems) {
        Node* node = model.findNode(dragItem.nodeId);
        if (node == nullptr) {
            continue;
        }

        node->position = dragItem.startPosition + delta;
    }
}

Node* DragController::hitTestNode(DiagramModel& model, const glm::vec2& mouseWorld) {
    for (auto it = model.nodes().rbegin(); it != model.nodes().rend(); ++it) {
        Node& node = *it;
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

void DragController::reset() {
    m_dragging = false;
    m_dragStartWorld = glm::vec2(0.0f);
    m_dragItems.clear();
}

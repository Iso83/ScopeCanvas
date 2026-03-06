#include "Interaction/SelectionController.h"

void SelectionController::onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld) {
    Node* hitNode = hitTest(model, mouseWorld);

    for (Node& node : model.nodes()) {
        node.selected = (&node == hitNode);
    }
}

Node* SelectionController::hitTest(DiagramModel& model, const glm::vec2& mouseWorld) {
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

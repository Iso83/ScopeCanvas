#include "SelectionController.h"

namespace Interaction {

std::optional<std::size_t> SelectionController::selectNodeAt(Engine::DiagramModel& model, const Engine::Vec2& worldPoint) const {
    std::optional<std::size_t> hitIndex;

    auto& nodes = model.nodes();
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (containsPoint(nodes[i], worldPoint)) {
            hitIndex = i;
            break;
        }
    }

    for (std::size_t i = 0; i < nodes.size(); ++i) {
        nodes[i].selected = hitIndex.has_value() && i == *hitIndex;
    }

    return hitIndex;
}

bool SelectionController::containsPoint(const Engine::Node& node, const Engine::Vec2& worldPoint) noexcept {
    const float minX = node.position.x;
    const float minY = node.position.y;
    const float maxX = node.position.x + node.size.x;
    const float maxY = node.position.y + node.size.y;

    return worldPoint.x >= minX && worldPoint.x <= maxX && worldPoint.y >= minY && worldPoint.y <= maxY;
}

} // namespace Interaction

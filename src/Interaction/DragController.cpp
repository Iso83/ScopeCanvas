#include "DragController.h"

namespace Interaction {

bool DragController::beginDrag(Engine::DiagramModel& model, const Engine::Vec2& worldPoint) {
    auto& nodes = model.nodes();
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (!nodes[i].selected) {
            continue;
        }

        m_draggedNodeIndex = i;
        m_grabOffset = {worldPoint.x - nodes[i].position.x, worldPoint.y - nodes[i].position.y};
        return true;
    }

    return false;
}

void DragController::updateDrag(Engine::DiagramModel& model, const Engine::Vec2& worldPoint) {
    if (!m_draggedNodeIndex.has_value()) {
        return;
    }

    auto& node = model.nodes()[*m_draggedNodeIndex];
    node.position = {worldPoint.x - m_grabOffset.x, worldPoint.y - m_grabOffset.y};
}

void DragController::endDrag() noexcept {
    m_draggedNodeIndex.reset();
}

} // namespace Interaction

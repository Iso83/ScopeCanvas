#pragma once

#include "Engine/DiagramModel.h"

#include <optional>

namespace Interaction {

class DragController {
public:
    bool beginDrag(Engine::DiagramModel& model, const Engine::Vec2& worldPoint);
    void updateDrag(Engine::DiagramModel& model, const Engine::Vec2& worldPoint);
    void endDrag() noexcept;

private:
    std::optional<std::size_t> m_draggedNodeIndex;
    Engine::Vec2 m_grabOffset {0.0F, 0.0F};
};

} // namespace Interaction

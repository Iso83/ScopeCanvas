#pragma once

#include "Engine/DiagramModel.h"

#include <optional>

namespace Interaction {

class SelectionController {
public:
    std::optional<std::size_t> selectNodeAt(Engine::DiagramModel& model, const Engine::Vec2& worldPoint) const;

private:
    [[nodiscard]] static bool containsPoint(const Engine::Node& node, const Engine::Vec2& worldPoint) noexcept;
};

} // namespace Interaction

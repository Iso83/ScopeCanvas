#pragma once

#include <glm/vec2.hpp>

namespace ScopeCanvas::Core {
struct GridSettings {
    bool enabled{};
    float cellSize{1.0F};

    [[nodiscard]] glm::vec2 snap(const glm::vec2& point) const {
        if (!enabled || cellSize <= 0.0F) {
            return point;
        }

        const auto snapAxis = [this](float value) -> float {
            const float scaled = value / cellSize;
            const int rounded = static_cast<int>(scaled >= 0.0F ? scaled + 0.5F : scaled - 0.5F);
            return static_cast<float>(rounded) * cellSize;
        };

        return glm::vec2{snapAxis(point.x), snapAxis(point.y)};
    }
};
} // namespace ScopeCanvas::Core

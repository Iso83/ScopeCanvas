#pragma once

#include "ScopeCanvasEngineCore/Core/Vec2.h"

namespace ScopeCanvas::Engine::Core
{
struct GridSettings
{
    bool enabled{};
    float cellSize{1.0F};

    [[nodiscard]] Vec2 snap(const Vec2& point) const
    {
        if (!enabled || cellSize <= 0.0F)
        {
            return point;
        }

        const auto snapAxis = [this](float value) -> float
        {
            const float scaled = value / cellSize;
            const int rounded = static_cast<int>(scaled >= 0.0F ? scaled + 0.5F : scaled - 0.5F);
            return static_cast<float>(rounded) * cellSize;
        };

        return Vec2{snapAxis(point.x), snapAxis(point.y)};
    }
};
} // namespace ScopeCanvas::Engine::Core

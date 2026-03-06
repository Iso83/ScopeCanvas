#pragma once

#include "Engine/DiagramModel.h"

namespace View {

class Camera2D {
public:
    void pan(float dx, float dy) noexcept;
    void setZoom(float zoom) noexcept;

    [[nodiscard]] float zoom() const noexcept;
    [[nodiscard]] const Engine::Vec2& position() const noexcept;
    [[nodiscard]] Engine::Vec2 worldToView(const Engine::Vec2& world) const noexcept;

private:
    Engine::Vec2 m_position {0.0F, 0.0F};
    float m_zoom {1.0F};
};

} // namespace View

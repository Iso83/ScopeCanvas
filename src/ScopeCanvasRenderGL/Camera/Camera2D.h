#pragma once

#include "ScopeCanvasEngineCore/Core/Vec2.h"

namespace ScopeCanvas::RenderGL::Camera
{
class Camera2D
{
public:
    void setZoom(float zoom);
    void setPosition(const Engine::Core::Vec2& position);

    [[nodiscard]] float zoom() const;
    [[nodiscard]] const Engine::Core::Vec2& position() const;

    [[nodiscard]] Engine::Core::Vec2 worldToScreen(const Engine::Core::Vec2& worldPoint) const;
    [[nodiscard]] Engine::Core::Vec2 screenToWorld(const Engine::Core::Vec2& screenPoint) const;

private:
    float m_zoom{1.0F};
    Engine::Core::Vec2 m_position{};
};
} // namespace ScopeCanvas::RenderGL::Camera

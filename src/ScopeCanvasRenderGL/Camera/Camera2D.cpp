#include "Camera/Camera2D.h"

#include <algorithm>

namespace ScopeCanvas::RenderGL::Camera
{
void Camera2D::setZoom(float zoom)
{
    m_zoom = std::max(zoom, 0.01F);
}

void Camera2D::setPosition(const Engine::Core::Vec2& position)
{
    m_position = position;
}

float Camera2D::zoom() const
{
    return m_zoom;
}

const Engine::Core::Vec2& Camera2D::position() const
{
    return m_position;
}

Engine::Core::Vec2 Camera2D::worldToScreen(const Engine::Core::Vec2& worldPoint) const
{
    return {(worldPoint.x - m_position.x) * m_zoom, (worldPoint.y - m_position.y) * m_zoom};
}

Engine::Core::Vec2 Camera2D::screenToWorld(const Engine::Core::Vec2& screenPoint) const
{
    return {screenPoint.x / m_zoom + m_position.x, screenPoint.y / m_zoom + m_position.y};
}
} // namespace ScopeCanvas::RenderGL::Camera

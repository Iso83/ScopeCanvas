#include "Camera2D.h"

#include <algorithm>

namespace View {

void Camera2D::pan(float dx, float dy) noexcept {
    m_position.x += dx;
    m_position.y += dy;
}

void Camera2D::setZoom(float zoom) noexcept {
    m_zoom = std::max(0.1F, zoom);
}

float Camera2D::zoom() const noexcept {
    return m_zoom;
}

const Engine::Vec2& Camera2D::position() const noexcept {
    return m_position;
}

Engine::Vec2 Camera2D::worldToView(const Engine::Vec2& world) const noexcept {
    return {(world.x - m_position.x) * m_zoom, (world.y - m_position.y) * m_zoom};
}

} // namespace View

#include <ScopeCanvas/render/camera/Camera2D.h>
#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace ScopeCanvas::Render::Camera {
Camera2D::Camera2D()
    : m_position(0.0F, 0.0F), m_zoom(1.0F), m_viewportWidth(1280), m_viewportHeight(720), m_projection(1.0F) {
    updateProjection();
}

void Camera2D::setViewportSize(int width, int height) {
    m_viewportWidth = std::max(width, 1);
    m_viewportHeight = std::max(height, 1);
    updateProjection();
}

void Camera2D::setPosition(const glm::vec2& position) {
    m_position = position;
}

const glm::vec2& Camera2D::position() const {
    return m_position;
}

void Camera2D::move(const glm::vec2& delta) {
    m_position += delta;
}

void Camera2D::setZoom(float zoom) {
    m_zoom = std::max(zoom, 0.05F);
    updateProjection();
}

float Camera2D::zoom() const {
    return m_zoom;
}

const glm::mat4& Camera2D::projection() const {
    return m_projection;
}

glm::mat4 Camera2D::view() const {
    return glm::translate(glm::mat4(1.0F), glm::vec3(-m_position, 0.0F));
}

glm::mat4 Camera2D::viewProjection() const {
    return m_projection * view();
}

glm::mat4 Camera2D::invViewProjection() const {
    return glm::inverse(viewProjection());
}

void Camera2D::updateProjection() {
    const float halfWidth = static_cast<float>(m_viewportWidth) * 0.5F / m_zoom;
    const float halfHeight = static_cast<float>(m_viewportHeight) * 0.5F / m_zoom;
    m_projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0F, 1.0F);
}
} // namespace ScopeCanvas::Render::Camera

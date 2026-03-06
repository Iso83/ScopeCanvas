#include "View/Camera2D.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <algorithm>

Camera2D::Camera2D()
    : m_position(0.0f, 0.0f),
      m_zoom(1.0f),
      m_viewportWidth(1280),
      m_viewportHeight(720),
      m_projection(1.0f) {
    updateProjection();
}

void Camera2D::setViewportSize(int width, int height) {
    m_viewportWidth = std::max(width, 1);
    m_viewportHeight = std::max(height, 1);
    updateProjection();
}

void Camera2D::move(const glm::vec2& delta) {
    m_position += delta;
}

void Camera2D::setZoom(float zoom) {
    m_zoom = std::max(zoom, 0.05f);
    updateProjection();
}

float Camera2D::zoom() const {
    return m_zoom;
}

const glm::mat4& Camera2D::projection() const {
    return m_projection;
}

glm::mat4 Camera2D::view() const {
    return glm::translate(glm::mat4(1.0f), glm::vec3(-m_position, 0.0f));
}

glm::mat4 Camera2D::viewProjection() const {
    return m_projection * view();
}

void Camera2D::updateProjection() {
    const float halfWidth = static_cast<float>(m_viewportWidth) * 0.5f / m_zoom;
    const float halfHeight = static_cast<float>(m_viewportHeight) * 0.5f / m_zoom;
    m_projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
}

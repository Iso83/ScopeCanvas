#include <ScopeCanvas/render/camera/Camera2D.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace ScopeCanvas::Render::Camera {
glm::mat4 Camera2D::view() const {
    return glm::translate(glm::mat4(1.0F), glm::vec3(-m_position, 0.0F));
}

void Camera2D::updateProjection() {
    const float halfWidth = static_cast<float>(m_viewportWidth) * 0.5F / m_zoom;
    const float halfHeight = static_cast<float>(m_viewportHeight) * 0.5F / m_zoom;
    m_projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0F, 1.0F);
}

} // namespace ScopeCanvas::Render::Camera

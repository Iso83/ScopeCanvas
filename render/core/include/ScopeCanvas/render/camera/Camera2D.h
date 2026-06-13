#pragma once

#include <algorithm>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace ScopeCanvas::Render::Camera {
class Camera2D {
  private:
    glm::vec2 m_position;
    float m_zoom;
    int m_viewportWidth;
    int m_viewportHeight;
    glm::mat4 m_projection;

  public:
    Camera2D()
        : m_position(0.0F, 0.0F), m_zoom(1.0F), m_viewportWidth(1280), m_viewportHeight(720), m_projection(1.0F) {
        updateProjection();
    }

    void setViewportSize(int width, int height) {
        m_viewportWidth = std::max(width, 1);
        m_viewportHeight = std::max(height, 1);
        updateProjection();
    }

    void setPosition(const glm::vec2& position) {
        m_position = position;
    }
    const glm::vec2& position() const {
        return m_position;
    }
    void move(const glm::vec2& delta) {
        m_position += delta;
    }

    float zoom() const {
        return m_zoom;
    }

    void setZoom(float zoom) {
        m_zoom = std::max(zoom, 0.05F);
        updateProjection();
    }

    const glm::mat4& projection() const {
        return m_projection;
    }

    glm::mat4 view() const;

    glm::mat4 viewProjection() const {
        return m_projection * view();
    }

    glm::mat4 invViewProjection() const {
        return glm::inverse(viewProjection());
    }

  private:
    void updateProjection();
};
} // namespace ScopeCanvas::Render::Camera

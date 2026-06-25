#pragma once

#include <algorithm>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace ScopeCanvas::Render::Camera {
class Camera2D {
  private:
    glm::vec2 m_position{};
    float m_zoom{1.0F};
    int m_viewportWidth{1};
    int m_viewportHeight{1};
    glm::mat4 m_projection{1.0F};

  public:
    Camera2D() {
        updateProjection();
    }

    void setViewportSize(int width, int height) {
        m_viewportWidth = std::max(width, 1);
        m_viewportHeight = std::max(height, 1);
        updateProjection();
    }

    int getViewportWidth() const noexcept {
        return m_viewportWidth;
    }

    int getViewportHeight() const noexcept {
        return m_viewportHeight;
    }

    const glm::vec2& position() const noexcept {
        return m_position;
    }

    void setPosition(const glm::vec2& position) {
        m_position = position;
    }

    void move(const glm::vec2& delta) {
        m_position += delta;
    }

    float zoom() const noexcept {
        return m_zoom;
    }

    void setZoom(float zoom) {
        m_zoom = std::max(zoom, 0.05F);
        updateProjection();
    }

    const glm::mat4& projection() const noexcept {
        return m_projection;
    }

    glm::mat4 view() const;

    glm::mat4 viewProjection() const noexcept {
        return m_projection * view();
    }

    glm::mat4 invViewProjection() const noexcept {
        return glm::inverse(viewProjection());
    }

  private:
    void updateProjection();
};
} // namespace ScopeCanvas::Render::Camera

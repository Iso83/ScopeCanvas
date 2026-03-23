#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace ScopeCanvas::Render::Camera {
class Camera2D {
  public:
    Camera2D();

    void setViewportSize(int width, int height);
    void setPosition(const glm::vec2& position);
    const glm::vec2& position() const;
    void move(const glm::vec2& delta);
    void setZoom(float zoom);
    float zoom() const;

    const glm::mat4& projection() const;
    glm::mat4 view() const;
    glm::mat4 viewProjection() const;
    glm::mat4 invViewProjection() const;

  private:
    glm::vec2 m_position;
    float m_zoom;
    int m_viewportWidth;
    int m_viewportHeight;
    glm::mat4 m_projection;

    void updateProjection();
};
} // namespace ScopeCanvas::Render::Camera

#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

class Camera2D {
public:
    Camera2D();

    void setViewportSize(int width, int height);
    void move(const glm::vec2& delta);
    void setZoom(float zoom);
    float zoom() const;

    const glm::mat4& projection() const;
    glm::mat4 view() const;
    glm::mat4 viewProjection() const;

private:
    void updateProjection();

    glm::vec2 m_position;
    float m_zoom;
    int m_viewportWidth;
    int m_viewportHeight;
    glm::mat4 m_projection;
};

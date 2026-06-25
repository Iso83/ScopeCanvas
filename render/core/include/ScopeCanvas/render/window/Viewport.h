#pragma once

#include <ScopeCanvas/render/camera/Camera2D.h>

namespace ScopeCanvas::Render::Window {
class DrawContext;

class Viewport {
  protected:
    Camera::Camera2D m_camera{};
    bool m_needsRender{false};

  public:
    const Camera::Camera2D& camera() const noexcept {
        return m_camera;
    }

    virtual void draw(DrawContext* ctx);

    inline virtual bool needsRender() {
        return m_needsRender;
    }

    inline void setViewportSize(int width, int height) {
        m_camera.setViewportSize(width, height);
        m_needsRender = true;
    }

    inline void setViewPosition(const glm::vec2 &position) {
        m_camera.setPosition(position);
        m_needsRender = true;
    }

    inline void moveView(const glm::vec2 &delta) {
        m_camera.move(delta);
        m_needsRender = true;
    }

    inline void setViewZoom(float zoom) {
        m_camera.setZoom(zoom);
        m_needsRender = true;
    }

    inline [[nodiscard]] glm::vec2 screenToWorld(float sx, float sy, float w, float h) const {
        const float nx = (sx / w) * 2.0F - 1.0F;
        const float ny = 1.0F - (sy / h) * 2.0F;
        const glm::mat4 inv = m_camera.invViewProjection();
        const glm::vec4 v = inv * glm::vec4(nx, ny, 0.0F, 1.0F);

        return {v.x / v.w, v.y / v.w};
    }

    inline [[nodiscard]] glm::vec2 screenToWorld(float sx, float sy) const {
        return screenToWorld(sx, sy, m_camera.getViewportWidth(), m_camera.getViewportHeight());
    }
};
} // namespace ScopeCanvas::Render::Window
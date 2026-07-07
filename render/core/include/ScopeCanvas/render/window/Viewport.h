#pragma once

#include <ScopeCanvas/core/helper/Attributes.h>
#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>
#include <vector>

namespace ScopeCanvas::Render::Window {
class DrawContext;

class Viewport : protected ViewportInteraction {
    friend class ViewportHandler;

  public:
    struct State {
        Camera::Camera2D m_camera{};
        DrawContext* drawContext{};
    };

  protected:
    glm::vec2 m_screenPosition{};

    //-------------------------------------------------------------------------
    // State
    //-------------------------------------------------------------------------
    std::vector<State> m_states{};
    static constexpr std::size_t InvalidState = std::numeric_limits<std::size_t>::max();
    std::size_t m_activeState{InvalidState};

    //-------------------------------------------------------------------------
    // Render
    //-------------------------------------------------------------------------
    bool m_needsRender{false};
    ViewportHandler* m_handler{};

  public:
    inline virtual void setScreenPosition(const glm::vec2& position) {
        m_screenPosition = position;
    }

    //-------------------------------------------------------------------------
    // State
    //-------------------------------------------------------------------------
    bool registerDrawContext(DrawContext* ctx, const bool activate = true);
    bool unregisterDrawContext(DrawContext* ctx);
    bool activateDrawContext(DrawContext* ctx);

  protected:
    inline State& activeState(std::size_t stateIndex) {
        assert(stateIndex < m_states.size());
        return m_states[stateIndex];
    }

    inline State& activeState() {
        return activeState(m_activeState);
    }

    inline const State& activeState(std::size_t stateIndex) const {
        assert(stateIndex < m_states.size());
        return m_states[stateIndex];
    }

    inline const State& activeState() const {
        return activeState(m_activeState);
    }

  public:
    //-------------------------------------------------------------------------
    // Render
    //-------------------------------------------------------------------------
    inline virtual void draw() {
        draw(activeState().drawContext);
    }

    inline virtual bool needsRender() {
        return m_needsRender;
    }

    inline ViewportHandler* handler() const {
        return m_handler;
    }

  protected:
    virtual void draw(DrawContext* ctx);

  public:
    //-------------------------------------------------------------------------
    // View
    //-------------------------------------------------------------------------
    SC_DEPRECATED("remove this func --> ref obj, UB (m_stats..alloc)")
    inline const Camera::Camera2D& camera() const {
        return activeState().m_camera;
    }

    void setViewportSize(int width, int height);
    void setViewPosition(const glm::vec2& position);
    void setViewZoom(float zoom);

    inline float viewZoom() const {
        return activeState().m_camera.zoom();
    }

    inline void moveView(const glm::vec2& delta) {
        if (delta.x == 0.0F && delta.y == 0.0F)
            return;

        activeState().m_camera.move(delta);
        m_needsRender = true;
    }

    [[nodiscard]] glm::vec2 screenToWorld(float sx, float sy, float w, float h) const;

    inline [[nodiscard]] glm::vec2 screenToWorld(float sx, float sy) const {
        auto cam = activeState().m_camera;
        return screenToWorld(sx - m_screenPosition.x, sy - m_screenPosition.y, cam.getViewportWidth(),
                             cam.getViewportHeight());
    }

  protected:
    //-------------------------------------------------------------------------
    // Interaction
    //-------------------------------------------------------------------------
    virtual bool handlesScroll() const override {
        return m_handler && m_handler->activeViewport() == this;
    }
    void onScroll(double xOffset, double yOffset) override;
};
} // namespace ScopeCanvas::Render::Window
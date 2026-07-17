#include <ScopeCanvas/render/gl/OpenGLApi.h>
#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/render/window/Viewport.h>

namespace ScopeCanvas::Render::Window {
//-------------------------------------------------------------------------
// State
//-------------------------------------------------------------------------
bool Viewport::registerDrawContext(DrawContext* ctx, const bool activate) {
    if (ctx == nullptr)
        return false;

    const auto it =
        std::find_if(m_states.begin(), m_states.end(), [ctx](const State& s) { return s.drawContext == ctx; });

    if (it != m_states.end())
        return false;

    m_states.emplace_back();
    m_states.back().drawContext = ctx;

    return activate ? activateDrawContext(ctx) : true;
}

bool Viewport::unregisterDrawContext(DrawContext* ctx) {
    const auto it =
        std::find_if(m_states.begin(), m_states.end(), [ctx](const State& s) { return s.drawContext == ctx; });

    if (it == m_states.end())
        return false;

    const std::size_t index = static_cast<std::size_t>(std::distance(m_states.begin(), it));

    m_states.erase(it);

    if (m_states.empty())
        m_activeState = 0;
    else if (m_activeState > index)
        --m_activeState;
    else if (m_activeState >= m_states.size())
        m_activeState = m_states.size() - 1;

    return true;
}

bool Viewport::activateDrawContext(DrawContext* ctx) {
    for (std::size_t i = 0; i < m_states.size(); ++i) {
        if (m_states[i].drawContext == ctx) {
            if (m_activeState == i)
                return false;

            m_activeState = i;
            m_needsRender = true;
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------
// Render
//-------------------------------------------------------------------------
void Viewport::draw(DrawContext* ctx) {
    GLint oldVp[4]{};
    glGetIntegerv(GL_VIEWPORT, oldVp);
    auto cam = activeState().m_camera;
    glViewport(0, 0, cam.getViewportWidth(), cam.getViewportHeight());
    ctx->draw(this);
    glViewport(oldVp[0], oldVp[1], oldVp[2], oldVp[3]);

    m_needsRender = false;
}

//-------------------------------------------------------------------------
// View
//-------------------------------------------------------------------------
void Viewport::setViewportSize(int width, int height) {
    auto& camera = activeState().m_camera;

    if (camera.getViewportWidth() == width && camera.getViewportHeight() == height)
        return;

    activeState().m_camera.setViewportSize(width, height);
    m_needsRender = true;
}

void Viewport::setViewPosition(const glm::vec2& position) {
    auto& camera = activeState().m_camera;

    if (camera.position() == position)
        return;

    camera.setPosition(position);
    m_needsRender = true;
}

void Viewport::setViewZoom(float zoom) {
    auto& camera = activeState().m_camera;

    if (camera.zoom() == zoom)
        return;

    camera.setZoom(zoom);
    m_needsRender = true;
}

glm::vec2 Viewport::screenToWorld(float sx, float sy, float w, float h) const {
    const float nx = (sx / w) * 2.0F - 1.0F;
    const float ny = 1.0F - (sy / h) * 2.0F;
    const glm::mat4 inv = activeState().m_camera.invViewProjection();
    const glm::vec4 v = inv * glm::vec4(nx, ny, 0.0F, 1.0F);

    return {v.x / v.w, v.y / v.w};
}

//-------------------------------------------------------------------------
// Interaction
//-------------------------------------------------------------------------
void Viewport::onScroll(double xOffset, double yOffset){
    if(yOffset != 0.0F)
        setViewZoom(std::max(0.05, activeState().m_camera.zoom() + yOffset * 0.1));
}
} // namespace ScopeCanvas::Render::Window
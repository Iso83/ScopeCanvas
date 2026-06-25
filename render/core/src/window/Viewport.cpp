#include <glad/glad.h>
#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/render/window/Viewport.h>

namespace ScopeCanvas::Render::Window {
void Viewport::draw(DrawContext* ctx) {
    GLint oldVp[4]{};
    glGetIntegerv(GL_VIEWPORT, oldVp);
    glViewport(0, 0, m_camera.getViewportWidth(), m_camera.getViewportHeight());
    ctx->draw(this);
    glViewport(oldVp[0], oldVp[1], oldVp[2], oldVp[3]);

    m_needsRender = false;
}
} // namespace ScopeCanvas::Render::Window
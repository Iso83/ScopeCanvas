#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/render/window/Canvas.h>
#include <glad/glad.h>

using namespace ScopeCanvas::Render;

namespace ScopeCanvas::Render::Window {
void Canvas::draw(DrawContext* ctx) {
    auto cam = m_states[m_activeState].m_camera;
    ensureRenderTarget(cam.getViewportWidth(), cam.getViewportHeight());

    GLint oldFb = 0;
    GLint oldVp[4]{};
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
    glGetIntegerv(GL_VIEWPORT, oldVp);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
    ctx->draw(this);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFb);
    glViewport(oldVp[0], oldVp[1], oldVp[2], oldVp[3]);

    m_needsRender = false;
}

void Canvas::ensureRenderTarget(int width, int height) {
    width = std::max(width, 1);
    height = std::max(height, 1);
    if (m_framebuffer != 0 && m_framebufferWidth == width && m_framebufferHeight == height)
        return;

    releaseRenderTarget();
    m_framebufferWidth = width;
    m_framebufferHeight = height;

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_framebufferWidth, m_framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

    glGenRenderbuffers(1, &m_depthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_framebufferWidth, m_framebufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencil);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::releaseRenderTarget() {
    if (m_depthStencil != 0)
        glDeleteRenderbuffers(1, &m_depthStencil);

    if (m_colorTexture != 0)
        glDeleteTextures(1, &m_colorTexture);

    if (m_framebuffer != 0)
        glDeleteFramebuffers(1, &m_framebuffer);

    m_depthStencil = 0;
    m_colorTexture = 0;
    m_framebuffer = 0;
}
} // namespace ScopeCanvas::Render::Window
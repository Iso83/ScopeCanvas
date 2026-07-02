#pragma once

#include <ScopeCanvas/render/window/Viewport.h>
#include <glad/glad.h>

namespace ScopeCanvas::Render::Window {
class Canvas : public Viewport {
  protected:
    GLuint m_framebuffer{0};
    GLuint m_colorTexture{0};
    GLuint m_depthStencil{0};
    int m_framebufferWidth{1};
    int m_framebufferHeight{1};

  public:
    ~Canvas() {
        releaseRenderTarget();
    }

    Viewport::draw;

    GLuint colorTexture() const noexcept {
        return m_colorTexture;
    }

  protected:
    void ensureRenderTarget(int width, int height);
    void releaseRenderTarget();
    virtual void draw(DrawContext* ctx);
};
} // namespace ScopeCanvas::Render::Window
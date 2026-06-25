#pragma once

#include <ScopeCanvas/render/window/Viewport.h>

namespace ScopeCanvas::Render::Window {
class DrawContext {
  public:
    virtual void draw(Window::Viewport* view) = 0;
    virtual bool needsRender() = 0;
};
} // namespace ScopeCanvas::Render::Window
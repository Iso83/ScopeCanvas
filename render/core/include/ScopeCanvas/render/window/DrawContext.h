#pragma once

#include <ScopeCanvas/render/window/Viewport.h>
#include <ScopeCanvas/render/window/ViewportInteraction.h>

namespace ScopeCanvas::Render::Window {
class ViewportHandler;

class DrawContext : protected ViewportInteraction {
    friend ViewportHandler;

  public:
    virtual void draw(Window::Viewport* view) = 0;
    virtual bool needsRender() = 0;
};
} // namespace ScopeCanvas::Render::Window
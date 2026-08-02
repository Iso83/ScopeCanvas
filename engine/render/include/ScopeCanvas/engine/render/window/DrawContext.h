#pragma once

#include <ScopeCanvas/engine/render/window/Viewport.h>
#include <ScopeCanvas/engine/render/window/ViewportInteraction.h>

namespace ScopeCanvas::Engine::Render::Window {
class ViewportHandler;

class DrawContext : protected ViewportInteraction {
    friend ViewportHandler;

public:
    virtual void draw(Window::Viewport* view) = 0;
    virtual bool needsRender() = 0;
};
} // namespace ScopeCanvas::Engine::Render::Window

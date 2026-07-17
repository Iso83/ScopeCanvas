#pragma once
#include <ScopeCanvas/render/window/Viewport.h>
class FlowViewport : public ScopeCanvas::Render::Window::Viewport {
  public:
    using ScopeCanvas::Render::Window::Viewport::draw;
    bool handlesKey() const override { return true; }
    void onKey(ScopeCanvas::Input::Key key, bool pressed) override;
};

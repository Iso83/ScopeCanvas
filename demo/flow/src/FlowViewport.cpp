#include "FlowViewport.h"
#include "FlowDrawContext.h"

namespace {
FlowDrawContext* flowContext(ScopeCanvas::Render::Window::Viewport::State& state) {
    return static_cast<FlowDrawContext*>(state.drawContext);
}
} // namespace

void FlowViewport::onKey(ScopeCanvas::Input::Key key, bool pressed) {
    if (!pressed || m_activeState == InvalidState)
        return;

    switch (key) {
    case ScopeCanvas::Input::Key::A:
    case ScopeCanvas::Input::Key::Left: moveView({-180.0F, 0.0F}); break;
    case ScopeCanvas::Input::Key::D:
    case ScopeCanvas::Input::Key::Right: moveView({180.0F, 0.0F}); break;
    case ScopeCanvas::Input::Key::W:
    case ScopeCanvas::Input::Key::Up: moveView({0.0F, 140.0F}); break;
    case ScopeCanvas::Input::Key::S:
    case ScopeCanvas::Input::Key::Down: moveView({0.0F, -140.0F}); break;
    default: return;
    }

    if (auto* ctx = flowContext(activeState()); ctx != nullptr)
        ctx->clampViewToContent(this);
    m_needsRender = true;
}

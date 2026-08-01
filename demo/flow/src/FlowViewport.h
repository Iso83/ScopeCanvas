#pragma once

#include <ScopeCanvas/engine/render/window/Viewport.h>

namespace ScopeCanvas::Demo::Flow {
class FlowViewport : public Engine::Render::Window::Viewport {
public:
    bool handlesKey() const override {
        return true;
    }
    void onKey(int key, bool pressed) override;
};
} // namespace ScopeCanvas::Demo::Flow

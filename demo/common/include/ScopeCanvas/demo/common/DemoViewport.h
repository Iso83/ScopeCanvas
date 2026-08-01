#pragma once

#include <ScopeCanvas/engine/render/window/Viewport.h>

namespace ScopeCanvas::Demo::Common {
class DemoViewport : public ScopeCanvas::Engine::Render::Window::Viewport {
private:
    double m_nowTime{}, m_lastTime{};

public:
    using ScopeCanvas::Engine::Render::Window::Viewport::draw;

    bool handlesKey() const override {
        return true;
    }
    void onKey(int key, bool pressed) override;

private:
    inline double deltaTime() const {
        constexpr double maxDelta = 0.025; // 25 ms
        return std::clamp(m_nowTime - m_lastTime, 0.0, maxDelta);
    }

    bool processOnDraw_KeyStroke();
    virtual void draw(ScopeCanvas::Engine::Render::Window::DrawContext* ctx) override;
};
} // namespace ScopeCanvas::Demo::Common

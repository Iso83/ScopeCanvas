#pragma once

#include <ScopeCanvas/render/window/Viewport.h>

class DemoViewport : public ScopeCanvas::Render::Window::Viewport {
  private:
    double m_nowTime{}, m_lastTime{};

  public:
    using ScopeCanvas::Render::Window::Viewport::draw;

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
    virtual void draw(ScopeCanvas::Render::Window::DrawContext* ctx) override;
};

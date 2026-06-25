#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

namespace ScopeCanvas::Render::Window {
class DrawContext;
class Viewport;
}

namespace ScopeCanvas::Render {
class RenderBenchmark {
  public:
    using Clock = std::chrono::steady_clock;

    struct Statistics {
        std::uint64_t renderedFrames{0};
        double elapsedSeconds{0.0};
        double framesPerSecond{0.0};
        double averageFrameTimeMs{0.0};
    };

  private:
    Statistics m_latest{};
    Clock::time_point m_intervalStart{};
    std::uint64_t m_intervalFrames{0};
    double m_intervalFrameTimeMs{0.0};
    mutable bool m_updated{false};
    std::vector<const Window::Viewport*> m_viewports{};

  public:
    void registerViewport(Window::Viewport* viewport);
    void unregisterViewport(Window::Viewport* viewport);

    void draw(Window::Viewport& viewport, Window::DrawContext& drawContext);

    [[nodiscard]] const Statistics &statistics() const noexcept {
        return m_latest;
    }
    [[nodiscard]] bool updated() const noexcept;
    [[nodiscard]] const std::vector<const Window::Viewport *> &viewports() const noexcept {
        return m_viewports;
    }
};
} // namespace ScopeCanvas::Render

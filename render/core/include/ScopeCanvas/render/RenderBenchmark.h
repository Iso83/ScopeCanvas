#pragma once

#include <chrono>
#include <cstdint>
#include <ScopeCanvas/render/window/ViewportHandler.h>

#ifdef SC_BUILD_DEMO_BENCHMARK
#define SC_SWAPINTERVAL 0
#else
#define SC_SWAPINTERVAL 1
#endif

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

        // Effective application throughput.
        double framesPerSecond{0.0};

        // Average render time per frame.
        double averageFrameTimeMs{0.0};

        // Estimated renderer throughput without waiting.
        double estimatedFramesPerSecond{0.0};

        // Time spent rendering during the interval.
        double renderLoadPercent{0.0};

        // Time spent outside rendering (wait/sleep/vsync).
        double idleTimeMs{0.0};
    };

  private:
    Statistics m_latest{};
    Clock::time_point m_intervalStart{};
    std::uint64_t m_intervalFrames{0};
    double m_intervalFrameTimeMs{0.0};
    bool m_updated{false};

  public:
    void draw(Window::ViewportHandler& viewHandlert);

    [[nodiscard]] const Statistics &statistics() const noexcept {
        return m_latest;
    }

    [[nodiscard]] bool updated() noexcept;
};
} // namespace ScopeCanvas::Render

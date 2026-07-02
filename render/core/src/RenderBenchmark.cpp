#include <ScopeCanvas/render/RenderBenchmark.h>
#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/render/window/Viewport.h>

namespace ScopeCanvas::Render {

    constexpr auto UPDATE_INTERVAL = std::chrono::seconds(1);

void RenderBenchmark::draw(Window::ViewportHandler& viewHandler) {
        if (m_intervalStart == Clock::time_point{})
            m_intervalStart = Clock::now();

        const auto frameStart = Clock::now();

        for (auto viewport : viewHandler.viewports()) 
                viewport->draw();

        const auto now = Clock::now();

        m_intervalFrameTimeMs += std::chrono::duration<double, std::milli>(now - frameStart).count();
        ++m_intervalFrames;

        const auto elapsed = now - m_intervalStart;
        if (elapsed < UPDATE_INTERVAL)
            return;

        const double elapsedSeconds = std::chrono::duration<double>(elapsed).count();

        m_latest.renderedFrames = m_intervalFrames;
        m_latest.elapsedSeconds = elapsedSeconds;
        m_latest.framesPerSecond = elapsedSeconds > 0.0 ? static_cast<double>(m_intervalFrames) / elapsedSeconds : 0.0;

        m_latest.averageFrameTimeMs =
            m_intervalFrames > 0 ? m_intervalFrameTimeMs / static_cast<double>(m_intervalFrames) : 0.0;

        m_intervalStart = now;
        m_intervalFrames = 0;
        m_intervalFrameTimeMs = 0.0;
        m_updated = true;
    }

bool RenderBenchmark::updated() noexcept {
    const bool result = m_updated;
    m_updated = false;
    return result;
}
} // namespace ScopeCanvas::Render

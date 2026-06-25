#include <ScopeCanvas/render/RenderBenchmark.h>

#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/render/window/Viewport.h>

#include <algorithm>

namespace ScopeCanvas::Render {

    constexpr auto UPDATE_INTERVAL = std::chrono::seconds(1);

void RenderBenchmark::registerViewport(Window::Viewport* viewport) {
    if (viewport == nullptr)
        return;

    if (m_intervalStart == Clock::time_point{})
        m_intervalStart = Clock::now();

    if (std::find(m_viewports.begin(), m_viewports.end(), viewport) == m_viewports.end())
        m_viewports.push_back(viewport);
}

void RenderBenchmark::unregisterViewport(Window::Viewport* viewport) {
    if (viewport == nullptr)
        return;

    const auto it = std::remove(m_viewports.begin(), m_viewports.end(), viewport);
    m_viewports.erase(it, m_viewports.end());
}

void RenderBenchmark::draw(Window::Viewport& viewport, Window::DrawContext& drawContext) {
    if (m_intervalStart == Clock::time_point{})
        m_intervalStart = Clock::now();

    const auto frameStart = Clock::now();
    viewport.draw(&drawContext);
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
    m_latest.averageFrameTimeMs = m_intervalFrames > 0
                                      ? m_intervalFrameTimeMs / static_cast<double>(m_intervalFrames)
                                      : 0.0;

    m_intervalStart = now;
    m_intervalFrames = 0;
    m_intervalFrameTimeMs = 0.0;
    m_updated = true;
}

bool RenderBenchmark::updated() const noexcept {
    const bool result = m_updated;
    m_updated = false;
    return result;
}
} // namespace ScopeCanvas::Render

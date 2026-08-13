#pragma once

namespace ScopeCanvas::Engine::Render::Text {
struct FontMetrics {
    float ascent{};
    float descent{};
    float lineHeight{};
    float glyphAdvance{};
    unsigned int pixelSize{};
};
} // namespace ScopeCanvas::Engine::Render::Text

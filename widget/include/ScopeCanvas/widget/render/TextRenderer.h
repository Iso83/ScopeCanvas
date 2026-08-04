#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace ScopeCanvas::Widget::Render {

struct FontMetrics {
    float ascent{};
    float descent{};
    float lineHeight{};
    unsigned int pixelSize{};
};

struct TextClipRect {
    glm::vec2 position{};
    glm::vec2 size{};
};

/**
 * Rasterizes and renders horizontal Latin-1 text in world coordinates.
 *
 * Positions passed to render() are baselines. ScopeCanvas world coordinates have
 * a positive-up Y axis. The view-projection matrix converts those coordinates to
 * the active framebuffer. pixelSize is the FreeType rasterization size in
 * framebuffer pixels; callers should reload the font when framebuffer scale
 * changes and choose worldScale to map raster pixels into world units. Clip
 * rectangles are world-coordinate, axis-aligned rectangles with an inclusive
 * lower-left position and an exclusive upper-right edge.
 */
class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    TextRenderer(TextRenderer&&) noexcept;
    TextRenderer& operator=(TextRenderer&&) noexcept;
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;

    bool init();
    void shutdown();
    bool loadFont(const std::string& path, unsigned int pixelSize);
    void releaseFont();

    [[nodiscard]] bool ready() const;
    [[nodiscard]] FontMetrics fontMetrics() const;
    [[nodiscard]] glm::vec2 measure(std::string_view text, float worldScale = 1.0F) const;
    void render(std::string_view text, glm::vec2 baseline, glm::vec4 color, const glm::mat4& viewProjection,
                std::optional<TextClipRect> clip = std::nullopt, float worldScale = 1.0F) const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ScopeCanvas::Widget::Render

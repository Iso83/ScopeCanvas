#pragma once

#include <ScopeCanvas/engine/render/text/ClipRect.h>
#include <ScopeCanvas/engine/render/text/FontMetrics.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace ScopeCanvas::Engine::Render::Text {
/**
 * Rasterizes and renders horizontal Latin-1 text in world coordinates.
 *
 * Uses FreeType rasterization with framebuffer-pixel font sizes and renders
 * glyphs into ScopeCanvas world coordinates using the supplied view projection.
 */
class Renderer {
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

public:
    Renderer();
    ~Renderer();
    Renderer(Renderer&&) noexcept;
    Renderer& operator=(Renderer&&) noexcept;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool init();
    void shutdown();
    bool loadFont(const std::string& path, unsigned int pixelSize);
    void releaseFont();

    [[nodiscard]] bool ready() const;
    [[nodiscard]] FontMetrics fontMetrics() const;
    [[nodiscard]] glm::vec2 measure(std::string_view text, float worldScale = 1.0F) const;
    void render(std::string_view text, glm::vec2 baseline, glm::vec4 color, const glm::mat4& viewProjection,
                std::optional<ClipRect> clip = std::nullopt, float worldScale = 1.0F) const;
};
} // namespace ScopeCanvas::Engine::Render::Text

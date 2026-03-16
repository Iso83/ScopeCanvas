#pragma once

#include <cstdint>

namespace ScopeCanvas::RenderGL::GL
{
class Texture
{
public:
    void resize(int width, int height);
    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;
    [[nodiscard]] std::uint32_t id() const;

private:
    int m_width{0};
    int m_height{0};
    std::uint32_t m_id{0};
};
} // namespace ScopeCanvas::RenderGL::GL

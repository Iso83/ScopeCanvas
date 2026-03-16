#include "GL/Texture.h"

namespace ScopeCanvas::RenderGL::GL
{
void Texture::resize(int width, int height)
{
    m_width = width;
    m_height = height;
    if (m_id == 0)
    {
        m_id = 1;
    }
}

int Texture::width() const
{
    return m_width;
}

int Texture::height() const
{
    return m_height;
}

std::uint32_t Texture::id() const
{
    return m_id;
}
} // namespace ScopeCanvas::RenderGL::GL

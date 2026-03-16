#pragma once

#include "GL/Texture.h"

namespace ScopeCanvas::RenderGL::GL
{
class Framebuffer
{
public:
    void attachColor(const Texture& texture);
    [[nodiscard]] bool isComplete() const;

private:
    bool m_complete{false};
};
} // namespace ScopeCanvas::RenderGL::GL

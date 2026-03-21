#include "gl/Framebuffer.h"

namespace ScopeCanvas::Render::GL {
void Framebuffer::attachColor(const Texture& texture) {
    m_complete = texture.id() != 0 && texture.width() > 0 && texture.height() > 0;
}

bool Framebuffer::isComplete() const {
    return m_complete;
}
} // namespace ScopeCanvas::Render::GL

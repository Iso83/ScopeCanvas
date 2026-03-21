#pragma once

#include "gl/Texture.h"

namespace ScopeCanvas::Render::GL {
class Framebuffer {
  public:
    void attachColor(const Texture& texture);
    [[nodiscard]] bool isComplete() const;

  private:
    bool m_complete{false};
};
} // namespace ScopeCanvas::Render::GL

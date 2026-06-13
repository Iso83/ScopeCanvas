#pragma once

#include <ScopeCanvas/render/gl/Shader.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace ScopeCanvas::Render::Renderers {
class SelectionRectRenderer {
  private:
    GL::Shader m_shader;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_fillEbo;
    GLuint m_outlineEbo;

  public:
    SelectionRectRenderer();
    ~SelectionRectRenderer();

    SelectionRectRenderer(const SelectionRectRenderer&) = delete;
    SelectionRectRenderer& operator=(const SelectionRectRenderer&) = delete;

    bool init();
    void shutdown();
    void render(const glm::mat4& viewProjection, const glm::vec2& start, const glm::vec2& end) const;

  private:
    void destroy();
};
} // namespace ScopeCanvas::Render::Renderers

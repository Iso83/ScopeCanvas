#pragma once

#include <ScopeCanvas/render/gl/Shader.h>

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Render {
class GridRenderer {
  private:
    GL::Shader m_shader;
    GLuint m_vao;
    GLuint m_vbo;

  public:
    GridRenderer();
    ~GridRenderer();

    GridRenderer(const GridRenderer&) = delete;
    GridRenderer& operator=(const GridRenderer&) = delete;

    bool init();
    void shutdown();
    void render(const Camera::Camera2D& camera, float cellSize) const;

  private:
    void destroy();
};
} // namespace ScopeCanvas::Render

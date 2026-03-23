#pragma once

#include <ScopeCanvas/render/gl/Shader.h>

#include <glm/mat4x4.hpp>
#include <vector>

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Render::Renderers {
class GridRenderer {
  public:
    GridRenderer();
    ~GridRenderer();

    GridRenderer(const GridRenderer&) = delete;
    GridRenderer& operator=(const GridRenderer&) = delete;

    bool init();
    void shutdown();
    void render(const Camera::Camera2D& camera, float cellSize) const;

  private:
    GL::Shader m_shader;
    GLuint m_vao;
    GLuint m_vbo;

    void uploadAndDrawLines(const std::vector<glm::vec2>& vertices, const glm::mat4& viewProjection,
                            const glm::vec3& color) const;
    void destroy();
};
} // namespace ScopeCanvas::Render::Renderers

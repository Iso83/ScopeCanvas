#pragma once

#include "gl/Shader.h"

#include <glad/glad.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace ScopeCanvas::Render::Renderers
{
class GridRenderer
{
public:
    GridRenderer();
    ~GridRenderer();

    GridRenderer(const GridRenderer&) = delete;
    GridRenderer& operator=(const GridRenderer&) = delete;

    bool init();
    void render(const glm::mat4& viewProjection, int viewportWidth, int viewportHeight, float cellSize);

private:
    void uploadAndDrawLines(const std::vector<glm::vec2>& vertices,
                            const glm::mat4& viewProjection,
                            const glm::vec3& color);
    void destroy();

    GL::Shader m_shader;
    GLuint m_vao;
    GLuint m_vbo;
};
} // namespace ScopeCanvas::Render::Renderers

#pragma once

#include "Render/Shader.h"

#include <glad/glad.h>

#include <glm/mat4x4.hpp>

class GridRenderer {
public:
    GridRenderer();
    ~GridRenderer();

    GridRenderer(const GridRenderer&) = delete;
    GridRenderer& operator=(const GridRenderer&) = delete;

    bool init(const char* vertexShaderPath, const char* fragmentShaderPath);
    void render(const glm::mat4& viewProjection, int viewportWidth, int viewportHeight, float cellSize) const;

private:
    void destroy();

    Shader m_shader;
    GLuint m_vao;
    GLuint m_vbo;
};

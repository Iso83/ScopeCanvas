#pragma once

#include "Render/Shader.h"

#include <glad/glad.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

class SelectionRectRenderer {
public:
    SelectionRectRenderer();
    ~SelectionRectRenderer();

    SelectionRectRenderer(const SelectionRectRenderer&) = delete;
    SelectionRectRenderer& operator=(const SelectionRectRenderer&) = delete;

    bool init(const char* vertexShaderPath, const char* fragmentShaderPath);
    void render(const glm::mat4& viewProjection, const glm::vec2& start, const glm::vec2& end) const;

private:
    void destroy();

    Shader m_shader;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_fillEbo;
    GLuint m_outlineEbo;
};

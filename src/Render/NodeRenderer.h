#pragma once

#include "Engine/DiagramModel.h"
#include "Render/Shader.h"

#include <glad/glad.h>

#include <glm/mat4x4.hpp>

class NodeRenderer {
public:
    NodeRenderer();
    ~NodeRenderer();

    NodeRenderer(const NodeRenderer&) = delete;
    NodeRenderer& operator=(const NodeRenderer&) = delete;

    bool init(const char* vertexShaderPath, const char* fragmentShaderPath);
    void render(const std::vector<Node>& nodes, const glm::mat4& viewProjection) const;

private:
    void destroy();

    Shader m_shader;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
};

#include "Render/NodeRenderer.h"

#include <glm/gtc/type_ptr.hpp>

NodeRenderer::NodeRenderer() : m_vao(0), m_vbo(0), m_ebo(0) {}

NodeRenderer::~NodeRenderer() {
    destroy();
}

bool NodeRenderer::init(const char* vertexShaderPath, const char* fragmentShaderPath) {
    if (!m_shader.loadFromFiles(vertexShaderPath, fragmentShaderPath)) {
        return false;
    }

    constexpr float quadVertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };

    constexpr GLuint quadIndices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    glBindVertexArray(0);
    return true;
}

void NodeRenderer::render(const std::vector<Node>& nodes, const glm::mat4& viewProjection) const {
    m_shader.use();

    const GLuint programId = m_shader.id();
    const GLint vpLoc = glGetUniformLocation(programId, "uViewProjection");
    const GLint posLoc = glGetUniformLocation(programId, "uNodePosition");
    const GLint sizeLoc = glGetUniformLocation(programId, "uNodeSize");
    const GLint colorLoc = glGetUniformLocation(programId, "uNodeColor");

    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));

    glBindVertexArray(m_vao);
    for (const Node& node : nodes) {
        glUniform2f(posLoc, node.position.x, node.position.y);
        glUniform2f(sizeLoc, node.size.x, node.size.y);

        if (node.selected) {
            glUniform3f(colorLoc, 0.95f, 0.67f, 0.25f);
        } else {
            glUniform3f(colorLoc, 0.22f, 0.27f, 0.34f);
        }

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
}

void NodeRenderer::destroy() {
    if (m_ebo != 0) {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }

    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

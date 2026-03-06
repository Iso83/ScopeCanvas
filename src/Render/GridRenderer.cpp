#include "Render/GridRenderer.h"

#include <glm/gtc/type_ptr.hpp>

GridRenderer::GridRenderer() : m_vao(0), m_vbo(0) {}

GridRenderer::~GridRenderer() {
    destroy();
}

bool GridRenderer::init(const char* vertexShaderPath, const char* fragmentShaderPath) {
    if (!m_shader.loadFromFiles(vertexShaderPath, fragmentShaderPath)) {
        return false;
    }

    constexpr float quadVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glBindVertexArray(0);

    return true;
}

void GridRenderer::render(const glm::mat4& viewProjection, int viewportWidth, int viewportHeight) const {
    m_shader.use();

    const GLuint programId = m_shader.id();
    const GLint vpLoc = glGetUniformLocation(programId, "uViewProjection");
    const GLint invVpLoc = glGetUniformLocation(programId, "uInvViewProjection");
    const GLint vpSizeLoc = glGetUniformLocation(programId, "uViewportSize");

    const glm::mat4 invViewProjection = glm::inverse(viewProjection);
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniformMatrix4fv(invVpLoc, 1, GL_FALSE, glm::value_ptr(invViewProjection));
    glUniform2f(vpSizeLoc, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight));

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void GridRenderer::destroy() {
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

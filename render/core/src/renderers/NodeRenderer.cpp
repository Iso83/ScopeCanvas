#include "ScopeCanvas/render/renderers/NodeRenderer.h"

#include "ScopeCanvas/render/camera/Camera2D.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace ScopeCanvas::Render::Renderers {
namespace {
unsigned int compile(unsigned int type, const char* src) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

struct NodePalette {
    glm::vec3 body;
    glm::vec3 header;
    glm::vec3 border;
    glm::vec3 connectorIn;
    glm::vec3 connectorOut;
};

NodePalette paletteForType(Core::NodeTypeId typeId) {
    switch (typeId.value()) {
    case 10:
        return {{0.20F, 0.22F, 0.29F},
                {0.11F, 0.13F, 0.18F},
                {0.64F, 0.70F, 0.86F},
                {0.80F, 0.84F, 0.95F},
                {0.97F, 0.80F, 0.27F}};
    case 11:
        return {{0.36F, 0.31F, 0.14F},
                {0.20F, 0.17F, 0.06F},
                {0.86F, 0.77F, 0.38F},
                {0.92F, 0.88F, 0.70F},
                {0.99F, 0.86F, 0.34F}};
    case 12:
        return {{0.16F, 0.29F, 0.46F},
                {0.09F, 0.16F, 0.27F},
                {0.50F, 0.75F, 0.98F},
                {0.78F, 0.89F, 0.98F},
                {0.55F, 0.79F, 1.00F}};
    case 13:
        return {{0.15F, 0.36F, 0.21F},
                {0.08F, 0.21F, 0.12F},
                {0.55F, 0.94F, 0.65F},
                {0.78F, 0.95F, 0.83F},
                {0.47F, 1.00F, 0.65F}};
    case 14:
        return {{0.43F, 0.16F, 0.16F},
                {0.24F, 0.08F, 0.08F},
                {0.98F, 0.56F, 0.56F},
                {0.97F, 0.83F, 0.83F},
                {1.00F, 0.54F, 0.54F}};
    case 15:
        return {{0.43F, 0.27F, 0.12F},
                {0.24F, 0.14F, 0.06F},
                {0.97F, 0.70F, 0.38F},
                {0.97F, 0.89F, 0.80F},
                {1.00F, 0.73F, 0.36F}};
    default:
        return {{0.22F, 0.27F, 0.34F},
                {0.13F, 0.17F, 0.22F},
                {0.83F, 0.86F, 0.92F},
                {0.86F, 0.88F, 0.95F},
                {0.92F, 0.94F, 0.98F}};
    }
}
} // namespace

bool NodeRenderer::init() {
    constexpr float quad[] = {
        0.0F, 0.0F, 1.0F, 0.0F, 1.0F, 1.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.0F, 1.0F,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    const char* vs = R"(#version 330 core
layout(location = 0) in vec2 aPos;

uniform mat4 uVP;
uniform vec2 uPos;
uniform vec2 uSize;

void main() {
    vec2 world = uPos + aPos * uSize;
    gl_Position = uVP * vec4(world, 0.0, 1.0);
})";

    const char* fs = R"(#version 330 core
out vec4 FragColor;
uniform vec4 uColor;
void main() {
    FragColor = uColor;
})";

    const unsigned int vertexShader = compile(GL_VERTEX_SHADER, vs);
    const unsigned int fragmentShader = compile(GL_FRAGMENT_SHADER, fs);

    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

void NodeRenderer::shutdown() {
    if (m_program != 0) {
        glDeleteProgram(m_program);
        m_program = 0;
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

void NodeRenderer::render(const std::vector<Scene::NodeRenderData>& nodes, const Camera::Camera2D& camera,
                          Core::CanvasNodeId selectedNodeId) const {
    if (nodes.empty()) {
        return;
    }

    glUseProgram(m_program);
    glBindVertexArray(m_vao);

    const glm::mat4 vp = camera.viewProjection();
    const int vpLoc = glGetUniformLocation(m_program, "uVP");
    const int posLoc = glGetUniformLocation(m_program, "uPos");
    const int sizeLoc = glGetUniformLocation(m_program, "uSize");
    const int colorLoc = glGetUniformLocation(m_program, "uColor");

    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, &vp[0][0]);

    for (const Scene::NodeRenderData& node : nodes) {
        const NodePalette palette = paletteForType(node.typeId);
        const bool selected = selectedNodeId.isValid() && node.id == selectedNodeId;

        glUniform2f(posLoc, node.position.x, node.position.y);
        glUniform2f(sizeLoc, node.size.x, node.size.y);
        glUniform4f(colorLoc, palette.body.r, palette.body.g, palette.body.b, 0.96F);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        const float titleHeight = std::min(node.size.y, 24.0F);
        glUniform2f(posLoc, node.position.x, node.position.y + node.size.y - titleHeight);
        glUniform2f(sizeLoc, node.size.x, titleHeight);
        glUniform4f(colorLoc, palette.header.r, palette.header.g, palette.header.b, 1.0F);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        constexpr float borderThickness = 1.5F;
        const glm::vec3 border = selected ? glm::vec3(0.95F, 0.67F, 0.25F) : palette.border;
        glUniform2f(posLoc, node.position.x, node.position.y);
        glUniform2f(sizeLoc, node.size.x, node.size.y);
        glUniform4f(colorLoc, border.r, border.g, border.b, 1.0F);

        constexpr float outline[] = {
            0.0F, 0.0F, 1.0F, 0.0F, 1.0F, 1.0F, 0.0F, 1.0F,
        };

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(outline), outline, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glLineWidth(selected ? 2.5F : borderThickness);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        constexpr float quad[] = {
            0.0F, 0.0F, 1.0F, 0.0F, 1.0F, 1.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.0F, 1.0F,
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        const float connectorSize = 8.0F;
        const float connectorOffset = connectorSize * 0.5F;
        const float count = static_cast<float>(node.connectorCount + 1U);
        const float step = node.size.y / count;
        for (std::uint32_t i = 0; i < node.connectorCount; ++i) {
            const bool output = (i % 2U) == 1U;
            const float cy = node.position.y + step * static_cast<float>(i + 1U) - connectorOffset;
            const float cx =
                output ? node.position.x + node.size.x - connectorOffset : node.position.x - connectorOffset;
            const glm::vec3 connectorColor = output ? palette.connectorOut : palette.connectorIn;
            glUniform2f(posLoc, cx, cy);
            glUniform2f(sizeLoc, connectorSize, connectorSize);
            glUniform4f(colorLoc, connectorColor.r, connectorColor.g, connectorColor.b, 1.0F);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    glBindVertexArray(0);
}

} // namespace ScopeCanvas::Render::Renderers

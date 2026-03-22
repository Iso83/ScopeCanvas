#include "ScopeCanvas/render/renderers/NodeRenderer.h"

#include "ScopeCanvas/render/camera/Camera2D.h"

#include <glad/glad.h>
#include <algorithm>
#include <glm/glm.hpp>

namespace ScopeCanvas::Render::Renderers {
namespace {
unsigned int compile(unsigned int type, const char* src) {
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

bool isSelected(Core::CanvasNodeId nodeId, const std::vector<Core::CanvasNodeId>& selectedNodeIds) {
    return std::find(selectedNodeIds.begin(), selectedNodeIds.end(), nodeId) != selectedNodeIds.end();
}
} // namespace

NodeRenderStyle NodeRenderer::defaultStyle(Core::NodeTypeId typeId) {
    switch (typeId.value()) {
    case 10:
        return {{0.12F, 0.14F, 0.18F, 0.96F}, {0.08F, 0.10F, 0.14F, 1.0F}, {0.36F, 0.43F, 0.56F, 1.0F},
                {0.86F, 0.63F, 0.27F, 1.0F}, 1.5F, 24.0F};
    case 11:
        return {{0.18F, 0.16F, 0.10F, 0.96F}, {0.13F, 0.11F, 0.07F, 1.0F}, {0.58F, 0.48F, 0.21F, 1.0F},
                {0.94F, 0.75F, 0.28F, 1.0F}, 1.5F, 24.0F};
    case 12:
        return {{0.10F, 0.16F, 0.23F, 0.96F}, {0.07F, 0.11F, 0.17F, 1.0F}, {0.25F, 0.53F, 0.76F, 1.0F},
                {0.51F, 0.82F, 1.0F, 1.0F}, 1.5F, 24.0F};
    case 13:
        return {{0.09F, 0.17F, 0.13F, 0.96F}, {0.06F, 0.12F, 0.09F, 1.0F}, {0.24F, 0.58F, 0.39F, 1.0F},
                {0.45F, 0.92F, 0.61F, 1.0F}, 1.5F, 24.0F};
    case 14:
        return {{0.20F, 0.11F, 0.12F, 0.96F}, {0.14F, 0.08F, 0.09F, 1.0F}, {0.62F, 0.29F, 0.33F, 1.0F},
                {0.98F, 0.58F, 0.56F, 1.0F}, 1.5F, 24.0F};
    case 15:
        return {{0.19F, 0.13F, 0.09F, 0.96F}, {0.14F, 0.09F, 0.06F, 1.0F}, {0.62F, 0.39F, 0.19F, 1.0F},
                {0.98F, 0.72F, 0.39F, 1.0F}, 1.5F, 24.0F};
    default:
        return {};
    }
}

bool NodeRenderer::init() {
    constexpr float quadVertices[] = {
        0.0F, 0.0F,
        1.0F, 0.0F,
        1.0F, 1.0F,
        0.0F, 1.0F,
    };
    constexpr unsigned int quadIndices[] = {0, 1, 2, 2, 3, 0};
    constexpr float outlineVertices[] = {
        0.0F, 0.0F,
        1.0F, 0.0F,
        1.0F, 1.0F,
        0.0F, 1.0F,
    };

    glGenVertexArrays(1, &m_fillVao);
    glGenBuffers(1, &m_fillVbo);
    glGenBuffers(1, &m_fillEbo);
    glBindVertexArray(m_fillVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_fillVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fillEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    glGenVertexArrays(1, &m_lineVao);
    glGenBuffers(1, &m_lineVbo);
    glBindVertexArray(m_lineVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(outlineVertices), outlineVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
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
    if (m_lineVbo != 0) {
        glDeleteBuffers(1, &m_lineVbo);
        m_lineVbo = 0;
    }
    if (m_lineVao != 0) {
        glDeleteVertexArrays(1, &m_lineVao);
        m_lineVao = 0;
    }
    if (m_fillEbo != 0) {
        glDeleteBuffers(1, &m_fillEbo);
        m_fillEbo = 0;
    }
    if (m_fillVbo != 0) {
        glDeleteBuffers(1, &m_fillVbo);
        m_fillVbo = 0;
    }
    if (m_fillVao != 0) {
        glDeleteVertexArrays(1, &m_fillVao);
        m_fillVao = 0;
    }
}

void NodeRenderer::render(const std::vector<Scene::NodeRenderData>& nodes, const Camera::Camera2D& camera,
                          const std::vector<Core::CanvasNodeId>& selectedNodeIds,
                          const StyleResolver& styleResolver) const {
    if (nodes.empty()) {
        return;
    }

    glUseProgram(m_program);

    const glm::mat4 vp = camera.viewProjection();
    const int vpLoc = glGetUniformLocation(m_program, "uVP");
    const int posLoc = glGetUniformLocation(m_program, "uPos");
    const int sizeLoc = glGetUniformLocation(m_program, "uSize");
    const int colorLoc = glGetUniformLocation(m_program, "uColor");

    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, &vp[0][0]);

    glBindVertexArray(m_fillVao);
    for (const Scene::NodeRenderData& node : nodes) {
        const NodeRenderStyle style = styleResolver ? styleResolver(node.typeId) : defaultStyle(node.typeId);
        const bool selected = isSelected(node.id, selectedNodeIds);
        const float headerHeight = std::clamp(style.headerHeight, 18.0F, node.size.y);

        glUniform2f(posLoc, node.position.x, node.position.y);
        glUniform2f(sizeLoc, node.size.x, node.size.y);
        glUniform4f(colorLoc, style.bodyColor.r, style.bodyColor.g, style.bodyColor.b, style.bodyColor.a);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glUniform2f(posLoc, node.position.x, node.position.y + node.size.y - headerHeight);
        glUniform2f(sizeLoc, node.size.x, headerHeight);
        glUniform4f(colorLoc, style.headerColor.r, style.headerColor.g, style.headerColor.b, style.headerColor.a);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        if (selected) {
            glUniform2f(posLoc, node.position.x - 2.0F, node.position.y - 2.0F);
            glUniform2f(sizeLoc, node.size.x + 4.0F, node.size.y + 4.0F);
            glUniform4f(colorLoc, style.selectionColor.r, style.selectionColor.g, style.selectionColor.b, 0.18F);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }
    }

    glBindVertexArray(m_lineVao);
    for (const Scene::NodeRenderData& node : nodes) {
        const NodeRenderStyle style = styleResolver ? styleResolver(node.typeId) : defaultStyle(node.typeId);
        const bool selected = isSelected(node.id, selectedNodeIds);
        const glm::vec4 border = selected ? style.selectionColor : style.borderColor;
        glUniform2f(posLoc, node.position.x, node.position.y);
        glUniform2f(sizeLoc, node.size.x, node.size.y);
        glUniform4f(colorLoc, border.r, border.g, border.b, border.a);
        glLineWidth(selected ? style.borderThickness + 1.5F : style.borderThickness);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

    glBindVertexArray(0);
}

} // namespace ScopeCanvas::Render::Renderers

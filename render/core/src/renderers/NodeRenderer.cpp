#include "ScopeCanvas/render/renderers/NodeRenderer.h"

#include "ScopeCanvas/render/camera/Camera2D.h"

#include <glad/glad.h>
#include <algorithm>
#include <cstdint>
#include <cctype>
#include <string>
#include <vector>

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

struct Glyph {
    std::uint8_t rows[7]{};
};

Glyph glyphFor(char c) {
    switch (static_cast<char>(std::toupper(static_cast<unsigned char>(c)))) {
    case 'A': return {{0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}};
    case 'B': return {{0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}};
    case 'C': return {{0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}};
    case 'D': return {{0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C}};
    case 'E': return {{0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}};
    case 'F': return {{0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}};
    case 'G': return {{0x0E, 0x11, 0x10, 0x10, 0x13, 0x11, 0x0E}};
    case 'H': return {{0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}};
    case 'I': return {{0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F}};
    case 'J': return {{0x01, 0x01, 0x01, 0x01, 0x11, 0x11, 0x0E}};
    case 'K': return {{0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}};
    case 'L': return {{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}};
    case 'M': return {{0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11}};
    case 'N': return {{0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11}};
    case 'O': return {{0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}};
    case 'P': return {{0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10}};
    case 'Q': return {{0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D}};
    case 'R': return {{0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}};
    case 'S': return {{0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E}};
    case 'T': return {{0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}};
    case 'U': return {{0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}};
    case 'V': return {{0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04}};
    case 'W': return {{0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A}};
    case 'X': return {{0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11}};
    case 'Y': return {{0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04}};
    case 'Z': return {{0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F}};
    case '0': return {{0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}};
    case '1': return {{0x04, 0x0C, 0x14, 0x04, 0x04, 0x04, 0x1F}};
    case '2': return {{0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F}};
    case '3': return {{0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E}};
    case '4': return {{0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}};
    case '5': return {{0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E}};
    case '6': return {{0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E}};
    case '7': return {{0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}};
    case '8': return {{0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}};
    case '9': return {{0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C}};
    case '[': return {{0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0E}};
    case ']': return {{0x0E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0E}};
    case '+': return {{0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00}};
    case '-': return {{0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}};
    case '>': return {{0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10}};
    case '*': return {{0x00, 0x15, 0x0E, 0x1F, 0x0E, 0x15, 0x00}};
    case '#': return {{0x0A, 0x0A, 0x1F, 0x0A, 0x1F, 0x0A, 0x0A}};
    case ' ': return {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    default: return {{0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04}};
    }
}

void appendQuad(std::vector<float>& vertices, float x0, float y0, float x1, float y1) {
    vertices.insert(vertices.end(), {x0, y0, x1, y0, x1, y1, x0, y0, x1, y1, x0, y1});
}

void appendTextGeometry(std::vector<float>& vertices, const std::string& text, const glm::vec2& origin, float pixelSize) {
    float penX = origin.x;
    const float penY = origin.y;
    const float glyphAdvance = pixelSize * 6.0F;

    for (char c : text) {
        const Glyph glyph = glyphFor(c);
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if ((glyph.rows[row] & (1U << (4 - col))) == 0U) {
                    continue;
                }
                const float x0 = penX + static_cast<float>(col) * pixelSize;
                const float y0 = penY + static_cast<float>(6 - row) * pixelSize;
                appendQuad(vertices, x0, y0, x0 + pixelSize, y0 + pixelSize);
            }
        }
        penX += glyphAdvance;
    }
}

std::string defaultTitle(Core::NodeTypeId typeId) {
    switch (typeId.value()) {
    case 1: return "NUMBER";
    case 2: return "ADD";
    case 3: return "MULTIPLY";
    case 4: return "OUTPUT";
    case 10: return "MESSAGE BLOCK";
    case 11: return "ROUND CONST";
    case 12: return "LOOP";
    case 13: return "CHOOSE";
    case 14: return "MIX";
    case 15: return "SIGMA";
    default: return "NODE";
    }
}
} // namespace

NodeRenderStyle NodeRenderer::defaultStyle(Core::NodeTypeId typeId) {
    switch (typeId.value()) {
    case 10:
        return {{0.12F, 0.14F, 0.18F, 0.96F}, {0.08F, 0.10F, 0.14F, 1.0F}, {0.36F, 0.43F, 0.56F, 1.0F},
                {0.86F, 0.63F, 0.27F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F}, 1.5F, 24.0F};
    case 11:
        return {{0.18F, 0.16F, 0.10F, 0.96F}, {0.13F, 0.11F, 0.07F, 1.0F}, {0.58F, 0.48F, 0.21F, 1.0F},
                {0.94F, 0.75F, 0.28F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F}, 1.5F, 24.0F};
    case 12:
        return {{0.10F, 0.16F, 0.23F, 0.96F}, {0.07F, 0.11F, 0.17F, 1.0F}, {0.25F, 0.53F, 0.76F, 1.0F},
                {0.51F, 0.82F, 1.0F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F}, 1.5F, 24.0F};
    case 13:
        return {{0.09F, 0.17F, 0.13F, 0.96F}, {0.06F, 0.12F, 0.09F, 1.0F}, {0.24F, 0.58F, 0.39F, 1.0F},
                {0.45F, 0.92F, 0.61F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F}, 1.5F, 24.0F};
    case 14:
        return {{0.20F, 0.11F, 0.12F, 0.96F}, {0.14F, 0.08F, 0.09F, 1.0F}, {0.62F, 0.29F, 0.33F, 1.0F},
                {0.98F, 0.58F, 0.56F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F}, 1.5F, 24.0F};
    case 15:
        return {{0.19F, 0.13F, 0.09F, 0.96F}, {0.14F, 0.09F, 0.06F, 1.0F}, {0.62F, 0.39F, 0.19F, 1.0F},
                {0.98F, 0.72F, 0.39F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F}, 1.5F, 24.0F};
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

    glGenVertexArrays(1, &m_textVao);
    glGenBuffers(1, &m_textVbo);
    glBindVertexArray(m_textVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_textVbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
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
    if (m_textVbo != 0) {
        glDeleteBuffers(1, &m_textVbo);
        m_textVbo = 0;
    }
    if (m_textVao != 0) {
        glDeleteVertexArrays(1, &m_textVao);
        m_textVao = 0;
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
                          const StyleResolver& styleResolver, const TitleResolver& titleResolver) const {
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
        const float headerHeight = std::clamp(style.headerHeight, 18.0F, node.size.y);

        glUniform2f(posLoc, node.position.x, node.position.y);
        glUniform2f(sizeLoc, node.size.x, node.size.y);
        glUniform4f(colorLoc, style.bodyColor.r, style.bodyColor.g, style.bodyColor.b, style.bodyColor.a);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glUniform2f(posLoc, node.position.x, node.position.y + node.size.y - headerHeight);
        glUniform2f(sizeLoc, node.size.x, headerHeight);
        glUniform4f(colorLoc, style.headerColor.r, style.headerColor.g, style.headerColor.b, style.headerColor.a);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glUniform2f(posLoc, node.position.x + 8.0F, node.position.y + node.size.y - headerHeight + 5.0F);
        glUniform2f(sizeLoc, 14.0F, std::max(headerHeight - 10.0F, 8.0F));
        glUniform4f(colorLoc, style.borderColor.r, style.borderColor.g, style.borderColor.b, 0.85F);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(m_textVao);
    for (const Scene::NodeRenderData& node : nodes) {
        const NodeRenderStyle style = styleResolver ? styleResolver(node.typeId) : defaultStyle(node.typeId);
        const float headerHeight = std::clamp(style.headerHeight, 18.0F, node.size.y);
        const std::string title = titleResolver ? titleResolver(node.typeId) : defaultTitle(node.typeId);
        const float pixelSize = std::clamp((node.size.x - 20.0F) / std::max(6.0F, static_cast<float>(title.size()) * 6.0F),
                                           1.2F, 2.2F);
        std::vector<float> textVertices;
        textVertices.reserve(title.size() * 7U * 5U * 12U);
        appendTextGeometry(textVertices, title, {node.position.x + 28.0F, node.position.y + node.size.y - headerHeight + 6.0F}, pixelSize);
        if (textVertices.empty()) {
            continue;
        }

        glUniform2f(posLoc, 0.0F, 0.0F);
        glUniform2f(sizeLoc, 1.0F, 1.0F);
        glUniform4f(colorLoc, style.textColor.r, style.textColor.g, style.textColor.b, style.textColor.a);
        glBindBuffer(GL_ARRAY_BUFFER, m_textVbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(textVertices.size() * sizeof(float)), textVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(textVertices.size() / 2U));
    }

    glBindVertexArray(m_lineVao);
    for (const Scene::NodeRenderData& node : nodes) {
        const NodeRenderStyle style = styleResolver ? styleResolver(node.typeId) : defaultStyle(node.typeId);
        const bool selected = isSelected(node.id, selectedNodeIds);
        const glm::vec4 border = selected ? style.selectionColor : style.borderColor;
        glUniform2f(posLoc, node.position.x, node.position.y);
        glUniform2f(sizeLoc, node.size.x, node.size.y);
        glUniform4f(colorLoc, border.r, border.g, border.b, border.a);
        glLineWidth(selected ? style.borderThickness + 2.0F : style.borderThickness);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

    glBindVertexArray(0);
}

} // namespace ScopeCanvas::Render::Renderers

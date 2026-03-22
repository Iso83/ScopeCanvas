#include "ScopeCanvas/render/renderers/NodeRenderer.h"

#include "ScopeCanvas/render/camera/Camera2D.h"

#include <glad/glad.h>
#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace ScopeCanvas::Render::Renderers {
namespace {
constexpr float kPi = 3.14159265358979323846F;

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

struct ColorVertex {
    float x;
    float y;
    float r;
    float g;
    float b;
    float a;
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
    case '?': return {{0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04}};
    case ' ': return {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    default: return {{0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04}};
    }
}

glm::vec4 mixColor(const glm::vec4& a, const glm::vec4& b, float t) {
    return a + (b - a) * t;
}

glm::vec4 withAlpha(const glm::vec4& color, float alphaScale) {
    return {color.r, color.g, color.b, color.a * alphaScale};
}

glm::vec4 brighten(const glm::vec4& color, float amount) {
    return mixColor(color, glm::vec4(1.0F), amount);
}

void appendTriangle(std::vector<ColorVertex>& vertices, const glm::vec2& a, const glm::vec4& colorA, const glm::vec2& b,
                    const glm::vec4& colorB, const glm::vec2& c, const glm::vec4& colorC) {
    vertices.push_back({a.x, a.y, colorA.r, colorA.g, colorA.b, colorA.a});
    vertices.push_back({b.x, b.y, colorB.r, colorB.g, colorB.b, colorB.a});
    vertices.push_back({c.x, c.y, colorC.r, colorC.g, colorC.b, colorC.a});
}

glm::vec4 bilerpColor(const std::array<glm::vec4, 4>& colors, const glm::vec2& point, const glm::vec2& minPoint,
                      const glm::vec2& maxPoint) {
    const float width = std::max(maxPoint.x - minPoint.x, 1.0F);
    const float height = std::max(maxPoint.y - minPoint.y, 1.0F);
    const float tx = std::clamp((point.x - minPoint.x) / width, 0.0F, 1.0F);
    const float ty = std::clamp((point.y - minPoint.y) / height, 0.0F, 1.0F);
    const glm::vec4 top = mixColor(colors[0], colors[1], tx);
    const glm::vec4 bottom = mixColor(colors[3], colors[2], tx);
    return mixColor(bottom, top, ty);
}

std::vector<glm::vec2> roundedRectPoints(const glm::vec2& pos, const glm::vec2& size, float radius, int segments) {
    const float clampedRadius = std::clamp(radius, 0.0F, std::min(size.x, size.y) * 0.5F);
    const glm::vec2 min = pos;
    const glm::vec2 max = pos + size;

    if (clampedRadius <= 0.01F) {
        return {
            {min.x, min.y},
            {max.x, min.y},
            {max.x, max.y},
            {min.x, max.y},
        };
    }

    const std::array<glm::vec2, 4> centers = {
        glm::vec2(min.x + clampedRadius, max.y - clampedRadius),
        glm::vec2(max.x - clampedRadius, max.y - clampedRadius),
        glm::vec2(max.x - clampedRadius, min.y + clampedRadius),
        glm::vec2(min.x + clampedRadius, min.y + clampedRadius),
    };
    const std::array<float, 5> startAngles = {
        kPi,
        kPi * 0.5F,
        0.0F,
        -kPi * 0.5F,
        kPi,
    };

    std::vector<glm::vec2> points;
    points.reserve(static_cast<std::size_t>(segments) * 4U + 4U);
    for (std::size_t corner = 0; corner < 4U; ++corner) {
        for (int i = 0; i <= segments; ++i) {
            if (corner > 0U && i == 0) {
                continue;
            }
            const float t = static_cast<float>(i) / static_cast<float>(segments);
            const float angle = startAngles[corner] + (startAngles[corner + 1] - startAngles[corner]) * t;
            points.push_back(centers[corner] + glm::vec2(std::cos(angle), std::sin(angle)) * clampedRadius);
        }
    }
    return points;
}

std::vector<glm::vec2> topRoundedRectPoints(const glm::vec2& pos, const glm::vec2& size, float radius, int segments) {
    const float clampedRadius = std::clamp(radius, 0.0F, std::min(size.x, size.y) * 0.5F);
    const glm::vec2 min = pos;
    const glm::vec2 max = pos + size;

    if (clampedRadius <= 0.01F) {
        return {
            {min.x, min.y},
            {max.x, min.y},
            {max.x, max.y},
            {min.x, max.y},
        };
    }

    std::vector<glm::vec2> points;
    points.reserve(static_cast<std::size_t>(segments) * 2U + 6U);
    points.push_back({min.x, min.y});
    points.push_back({max.x, min.y});

    const glm::vec2 topRightCenter(max.x - clampedRadius, max.y - clampedRadius);
    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const float angle = 0.0F + (kPi * 0.5F) * t;
        points.push_back(topRightCenter + glm::vec2(std::cos(angle), std::sin(angle)) * clampedRadius);
    }

    const glm::vec2 topLeftCenter(min.x + clampedRadius, max.y - clampedRadius);
    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const float angle = kPi * 0.5F + (kPi * 0.5F) * t;
        points.push_back(topLeftCenter + glm::vec2(std::cos(angle), std::sin(angle)) * clampedRadius);
    }

    return points;
}

void appendConvexPolygon(std::vector<ColorVertex>& vertices, const std::vector<glm::vec2>& points,
                         const std::array<glm::vec4, 4>& colors, const glm::vec2& minPoint, const glm::vec2& maxPoint) {
    if (points.size() < 3U) {
        return;
    }
    glm::vec2 center(0.0F);
    for (const glm::vec2& point : points) {
        center += point;
    }
    center /= static_cast<float>(points.size());
    const glm::vec4 centerColor = bilerpColor(colors, center, minPoint, maxPoint);
    for (std::size_t i = 0; i < points.size(); ++i) {
        const glm::vec2& a = points[i];
        const glm::vec2& b = points[(i + 1U) % points.size()];
        appendTriangle(vertices, center, centerColor, a, bilerpColor(colors, a, minPoint, maxPoint), b,
                       bilerpColor(colors, b, minPoint, maxPoint));
    }
}

void appendTextQuad(std::vector<ColorVertex>& vertices, float x0, float y0, float x1, float y1, const glm::vec4& color) {
    appendTriangle(vertices, {x0, y0}, color, {x1, y0}, color, {x1, y1}, color);
    appendTriangle(vertices, {x0, y0}, color, {x1, y1}, color, {x0, y1}, color);
}

void appendTextGeometry(std::vector<ColorVertex>& vertices, const std::string& text, const glm::vec2& origin, float pixelSize,
                        float glyphAdvance, const glm::vec4& color) {
    float penX = origin.x;
    const float penY = origin.y;

    for (char c : text) {
        const Glyph glyph = glyphFor(c);
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if ((glyph.rows[row] & (1U << (4 - col))) == 0U) {
                    continue;
                }
                const float x0 = penX + static_cast<float>(col) * pixelSize;
                const float y0 = penY + static_cast<float>(6 - row) * pixelSize;
                appendTextQuad(vertices, x0, y0, x0 + pixelSize, y0 + pixelSize, color);
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

std::string defaultIcon(Core::NodeTypeId typeId) {
    switch (typeId.value()) {
    case 1: return "1";
    case 2: return "+";
    case 3: return "*";
    case 4: return "O";
    case 10: return "[]";
    case 11: return "b";
    case 12: return "L";
    case 13: return "T";
    case 14: return "F";
    case 15: return ">>";
    default: return "N";
    }
}
} // namespace

NodeRenderStyle NodeRenderer::defaultStyle(Core::NodeTypeId typeId) {
    switch (typeId.value()) {
    case 10:
        return {{0.12F, 0.14F, 0.18F, 0.96F}, {0.09F, 0.11F, 0.16F, 1.0F}, {0.34F, 0.42F, 0.58F, 0.95F},
                {0.36F, 0.43F, 0.56F, 1.0F}, {0.86F, 0.63F, 0.27F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F},
                {0.94F, 0.96F, 1.0F, 1.0F}, 1.5F, 28.0F, 12.0F};
    case 11:
        return {{0.18F, 0.16F, 0.10F, 0.96F}, {0.13F, 0.11F, 0.07F, 1.0F}, {0.66F, 0.54F, 0.20F, 0.95F},
                {0.58F, 0.48F, 0.21F, 1.0F}, {0.94F, 0.75F, 0.28F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F},
                {1.0F, 0.95F, 0.80F, 1.0F}, 1.5F, 28.0F, 12.0F};
    case 12:
        return {{0.10F, 0.16F, 0.23F, 0.96F}, {0.07F, 0.11F, 0.17F, 1.0F}, {0.28F, 0.52F, 0.82F, 0.95F},
                {0.25F, 0.53F, 0.76F, 1.0F}, {0.51F, 0.82F, 1.0F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F},
                {0.90F, 0.97F, 1.0F, 1.0F}, 1.5F, 28.0F, 12.0F};
    case 13:
        return {{0.09F, 0.17F, 0.13F, 0.96F}, {0.06F, 0.12F, 0.09F, 1.0F}, {0.28F, 0.58F, 0.41F, 0.95F},
                {0.24F, 0.58F, 0.39F, 1.0F}, {0.45F, 0.92F, 0.61F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F},
                {0.92F, 1.0F, 0.95F, 1.0F}, 1.5F, 28.0F, 12.0F};
    case 14:
        return {{0.20F, 0.11F, 0.12F, 0.96F}, {0.14F, 0.08F, 0.09F, 1.0F}, {0.72F, 0.28F, 0.37F, 0.95F},
                {0.62F, 0.29F, 0.33F, 1.0F}, {0.98F, 0.58F, 0.56F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F},
                {1.0F, 0.93F, 0.94F, 1.0F}, 1.5F, 28.0F, 12.0F};
    case 15:
        return {{0.19F, 0.13F, 0.09F, 0.96F}, {0.14F, 0.09F, 0.06F, 1.0F}, {0.72F, 0.46F, 0.21F, 0.95F},
                {0.62F, 0.39F, 0.19F, 1.0F}, {0.98F, 0.72F, 0.39F, 1.0F}, {0.95F, 0.96F, 0.98F, 1.0F},
                {1.0F, 0.94F, 0.86F, 1.0F}, 1.5F, 28.0F, 12.0F};
    default:
        return {};
    }
}

bool NodeRenderer::init() {
    glGenVertexArrays(1, &m_fillVao);
    glGenBuffers(1, &m_fillVbo);
    glBindVertexArray(m_fillVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_fillVbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), reinterpret_cast<void*>(2 * sizeof(float)));

    glGenVertexArrays(1, &m_lineVao);
    glGenBuffers(1, &m_lineVbo);
    glBindVertexArray(m_lineVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), reinterpret_cast<void*>(2 * sizeof(float)));

    glGenVertexArrays(1, &m_textVao);
    glGenBuffers(1, &m_textVbo);
    glBindVertexArray(m_textVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_textVbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), reinterpret_cast<void*>(2 * sizeof(float)));
    glBindVertexArray(0);

    const char* vs = R"(#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;

uniform mat4 uVP;
out vec4 vColor;

void main() {
    gl_Position = uVP * vec4(aPos, 0.0, 1.0);
    vColor = aColor;
})";

    const char* fs = R"(#version 330 core
in vec4 vColor;
out vec4 FragColor;

void main() {
    FragColor = vColor;
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
                          const std::vector<Core::CanvasNodeId>& selectedNodeIds, const StyleResolver& styleResolver,
                          const TitleResolver& titleResolver, const IconResolver& iconResolver) const {
    if (nodes.empty()) {
        return;
    }

    glUseProgram(m_program);

    const glm::mat4 vp = camera.viewProjection();
    const int vpLoc = glGetUniformLocation(m_program, "uVP");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, &vp[0][0]);

    glBindVertexArray(m_fillVao);
    for (const Scene::NodeRenderData& node : nodes) {
        const NodeRenderStyle style = styleResolver ? styleResolver(node.typeId) : defaultStyle(node.typeId);
        const bool selected = isSelected(node.id, selectedNodeIds);
        const float headerHeight = std::clamp(style.headerHeight, 22.0F, node.size.y - 8.0F);
        const float borderThickness = std::clamp(selected ? style.borderThickness + 1.1F : style.borderThickness, 1.0F, 3.5F);
        const float cornerRadius = std::clamp(style.cornerRadius, 4.0F, std::min(node.size.x, node.size.y) * 0.22F);
        const glm::vec4 border = selected ? style.selectionColor : style.borderColor;

        std::vector<ColorVertex> fillVertices;
        fillVertices.reserve(512U);

        const std::array<glm::vec4, 4> borderColors = {
            brighten(border, selected ? 0.18F : 0.10F),
            brighten(border, selected ? 0.12F : 0.06F),
            withAlpha(border, 0.92F),
            withAlpha(border, 0.95F),
        };
        appendConvexPolygon(fillVertices, roundedRectPoints(node.position, node.size, cornerRadius, 8), borderColors, node.position,
                            node.position + node.size);

        const glm::vec2 innerPos = node.position + glm::vec2(borderThickness);
        const glm::vec2 innerSize = node.size - glm::vec2(borderThickness * 2.0F);
        const float innerRadius = std::max(cornerRadius - borderThickness, 2.0F);

        const glm::vec4 bodyTop = brighten(style.bodyColor, 0.05F);
        const glm::vec4 bodyBottom = withAlpha(style.bodyColor, 0.96F);
        const std::array<glm::vec4, 4> bodyColors = {bodyTop, bodyTop, bodyBottom, bodyBottom};
        appendConvexPolygon(fillVertices, roundedRectPoints(innerPos, innerSize, innerRadius, 8), bodyColors, innerPos,
                            innerPos + innerSize);

        const glm::vec2 headerPos = {innerPos.x, innerPos.y + innerSize.y - headerHeight};
        const glm::vec2 headerSize = {innerSize.x, headerHeight};
        const glm::vec4 headerLeft = brighten(style.headerColor, 0.06F);
        const glm::vec4 headerRight = brighten(style.headerAccentColor, 0.10F);
        const glm::vec4 headerBottomLeft = withAlpha(mixColor(style.headerColor, style.bodyColor, 0.28F), 0.98F);
        const glm::vec4 headerBottomRight = withAlpha(mixColor(style.headerAccentColor, style.bodyColor, 0.18F), 0.98F);
        const std::array<glm::vec4, 4> headerColors = {headerLeft, headerRight, headerBottomRight, headerBottomLeft};
        appendConvexPolygon(fillVertices, topRoundedRectPoints(headerPos, headerSize, innerRadius, 8), headerColors, headerPos,
                            headerPos + headerSize);

        const glm::vec2 sheenPos = {headerPos.x + 2.0F, headerPos.y + headerSize.y * 0.58F};
        const glm::vec2 sheenSize = {headerSize.x - 4.0F, std::max(headerSize.y * 0.24F, 3.0F)};
        const std::array<glm::vec4, 4> sheenColors = {
            withAlpha(brighten(style.headerAccentColor, 0.45F), 0.18F),
            withAlpha(brighten(style.headerAccentColor, 0.20F), 0.04F),
            withAlpha(brighten(style.headerAccentColor, 0.20F), 0.0F),
            withAlpha(brighten(style.headerAccentColor, 0.35F), 0.06F),
        };
        appendConvexPolygon(fillVertices, topRoundedRectPoints(sheenPos, sheenSize, std::max(innerRadius - 3.0F, 2.0F), 6), sheenColors,
                            sheenPos, sheenPos + sheenSize);

        const glm::vec2 iconPos = {headerPos.x + 10.0F, headerPos.y + 5.0F};
        const glm::vec2 iconSize = {17.0F, std::max(headerHeight - 10.0F, 12.0F)};
        const glm::vec4 iconBgLeft = withAlpha(brighten(style.headerAccentColor, 0.08F), 0.92F);
        const glm::vec4 iconBgRight = withAlpha(brighten(style.borderColor, 0.16F), 0.88F);
        const std::array<glm::vec4, 4> iconColors = {iconBgLeft, iconBgRight, withAlpha(iconBgRight, 0.78F), withAlpha(iconBgLeft, 0.78F)};
        appendConvexPolygon(fillVertices, roundedRectPoints(iconPos, iconSize, 4.5F, 6), iconColors, iconPos, iconPos + iconSize);

        const glm::vec2 underlinePos = {headerPos.x + 8.0F, headerPos.y + 2.0F};
        const glm::vec2 underlineSize = {headerSize.x - 16.0F, 1.75F};
        const std::array<glm::vec4, 4> underlineColors = {
            withAlpha(brighten(style.headerAccentColor, 0.35F), 0.24F),
            withAlpha(brighten(style.headerAccentColor, 0.15F), 0.04F),
            withAlpha(style.headerAccentColor, 0.0F),
            withAlpha(style.headerAccentColor, 0.08F),
        };
        appendConvexPolygon(fillVertices, roundedRectPoints(underlinePos, underlineSize, 0.8F, 3), underlineColors, underlinePos,
                            underlinePos + underlineSize);

        glBindBuffer(GL_ARRAY_BUFFER, m_fillVbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(fillVertices.size() * sizeof(ColorVertex)), fillVertices.data(),
                     GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(fillVertices.size()));
    }

    glBindVertexArray(m_textVao);
    for (const Scene::NodeRenderData& node : nodes) {
        const NodeRenderStyle style = styleResolver ? styleResolver(node.typeId) : defaultStyle(node.typeId);
        const float headerHeight = std::clamp(style.headerHeight, 22.0F, node.size.y - 8.0F);
        const std::string title = titleResolver ? titleResolver(node.typeId) : defaultTitle(node.typeId);
        const std::string icon = iconResolver ? iconResolver(node.typeId) : defaultIcon(node.typeId);

        const float usableWidth = std::max(node.size.x - 58.0F, 42.0F);
        const float pixelSize =
            std::clamp(usableWidth / std::max(8.0F, static_cast<float>(title.size()) * 5.8F), 1.3F, 2.35F);
        const float glyphAdvance = pixelSize * 5.7F;
        const float iconPixelSize = std::clamp(pixelSize * 0.92F, 1.2F, 2.1F);

        std::vector<ColorVertex> textVertices;
        textVertices.reserve((title.size() + icon.size()) * 7U * 5U * 12U * 3U);

        const glm::vec2 iconOrigin = {node.position.x + 13.0F, node.position.y + node.size.y - headerHeight + 7.0F};
        const glm::vec2 titleOrigin = {node.position.x + 34.0F, node.position.y + node.size.y - headerHeight + 7.0F};
        const glm::vec4 shadowColor(0.01F, 0.02F, 0.03F, 0.55F);
        appendTextGeometry(textVertices, icon, iconOrigin + glm::vec2(0.55F, -0.45F), iconPixelSize, iconPixelSize * 5.6F,
                           shadowColor);
        appendTextGeometry(textVertices, title, titleOrigin + glm::vec2(0.8F, -0.55F), pixelSize, glyphAdvance, shadowColor);

        appendTextGeometry(textVertices, icon, iconOrigin, iconPixelSize, iconPixelSize * 5.6F, style.iconColor);
        appendTextGeometry(textVertices, title, titleOrigin, pixelSize, glyphAdvance, style.textColor);
        appendTextGeometry(textVertices, title, titleOrigin + glm::vec2(0.32F, 0.0F), pixelSize, glyphAdvance,
                           withAlpha(brighten(style.textColor, 0.10F), 0.18F));

        if (textVertices.empty()) {
            continue;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_textVbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(textVertices.size() * sizeof(ColorVertex)), textVertices.data(),
                     GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(textVertices.size()));
    }

    glBindVertexArray(m_lineVao);
    for (const Scene::NodeRenderData& node : nodes) {
        const NodeRenderStyle style = styleResolver ? styleResolver(node.typeId) : defaultStyle(node.typeId);
        const bool selected = isSelected(node.id, selectedNodeIds);
        const float cornerRadius = std::clamp(style.cornerRadius, 4.0F, std::min(node.size.x, node.size.y) * 0.22F);
        const glm::vec4 border = selected ? brighten(style.selectionColor, 0.08F) : brighten(style.borderColor, 0.04F);
        const std::vector<glm::vec2> outline = roundedRectPoints(node.position, node.size, cornerRadius, 8);
        std::vector<ColorVertex> outlineVertices;
        outlineVertices.reserve(outline.size());
        for (const glm::vec2& point : outline) {
            outlineVertices.push_back({point.x, point.y, border.r, border.g, border.b, border.a});
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(outlineVertices.size() * sizeof(ColorVertex)), outlineVertices.data(),
                     GL_DYNAMIC_DRAW);
        glLineWidth(selected ? style.borderThickness + 2.0F : style.borderThickness + 0.45F);
        glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(outlineVertices.size()));
    }

    glBindVertexArray(0);
}

} // namespace ScopeCanvas::Render::Renderers

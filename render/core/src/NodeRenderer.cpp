#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/render/geometry/RoundedRect.h>
#include <ScopeCanvas/render/NodeRenderer.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <glad/glad.h>
#include <vector>

namespace ScopeCanvas::Render {
namespace {
unsigned int compile(unsigned int type, const char* src) {
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

using namespace ScopeCanvas::Core::Ids;

bool isSelected(NodeId nodeId, const std::vector<NodeId>& selectedNodeIds) {
    return std::find(selectedNodeIds.begin(), selectedNodeIds.end(), nodeId) != selectedNodeIds.end();
}

struct ColorVertex {
    float x;
    float y;
    float r;
    float g;
    float b;
    float a;
};

glm::vec4 mixColor(const glm::vec4& a, const glm::vec4& b, float t) {
    return a + (b - a) * t;
}

glm::vec4 opaque(const glm::vec4& color) {
    return {color.r, color.g, color.b, 1.0F};
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
    return ScopeCanvas::Render::Geometry::roundedRectOutline(pos, size, radius, segments);
}

float safeCornerRadius(float requestedRadius, const glm::vec2& size) {
    const float maxRadius = std::max(std::min(size.x, size.y) * 0.22F, 0.0F);
    const float minRadius = std::min(4.0F, maxRadius);
    return std::clamp(requestedRadius, minRadius, maxRadius);
}

void appendConvexPolygon(std::vector<ColorVertex>& vertices, const std::vector<glm::vec2>& points,
                         const std::array<glm::vec4, 4>& colors, const glm::vec2& minPoint, const glm::vec2& maxPoint) {
    if (points.size() < 3U)
        return;

    glm::vec2 center(0.0F);
    for (const glm::vec2& point : points)
        center += point;

    center /= static_cast<float>(points.size());
    const glm::vec4 centerColor = bilerpColor(colors, center, minPoint, maxPoint);
    for (std::size_t i = 0; i < points.size(); ++i) {
        const glm::vec2& a = points[i];
        const glm::vec2& b = points[(i + 1U) % points.size()];
        appendTriangle(vertices, center, centerColor, a, bilerpColor(colors, a, minPoint, maxPoint), b,
                       bilerpColor(colors, b, minPoint, maxPoint));
    }
}

void appendBorderRing(std::vector<ColorVertex>& vertices, const std::vector<glm::vec2>& outer,
                      const std::vector<glm::vec2>& inner, const std::array<glm::vec4, 4>& colors,
                      const glm::vec2& minPoint, const glm::vec2& maxPoint) {
    if (outer.size() < 3U || outer.size() != inner.size())
        return;

    for (std::size_t i = 0; i < outer.size(); ++i) {
        const glm::vec2& outerA = outer[i];
        const glm::vec2& outerB = outer[(i + 1U) % outer.size()];
        const glm::vec2& innerA = inner[i];
        const glm::vec2& innerB = inner[(i + 1U) % inner.size()];

        appendTriangle(vertices, outerA, bilerpColor(colors, outerA, minPoint, maxPoint), innerA,
                       bilerpColor(colors, innerA, minPoint, maxPoint), innerB,
                       bilerpColor(colors, innerB, minPoint, maxPoint));
        appendTriangle(vertices, outerA, bilerpColor(colors, outerA, minPoint, maxPoint), innerB,
                       bilerpColor(colors, innerB, minPoint, maxPoint), outerB,
                       bilerpColor(colors, outerB, minPoint, maxPoint));
    }
}

} // namespace

NodeRenderStyle NodeRenderer::defaultStyle(NodeTypeId /*typeId*/) {
    return {};
}

glm::vec2 NodeRenderer::nodeSize(const Scene::NodeRenderData& node) const {
    return node.size;
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
                          const std::vector<NodeId>& selectedNodeIds, const StyleResolver& styleResolver) const {
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
        const float borderThickness =
            std::clamp(selected ? style.borderThickness + 1.1F : style.borderThickness, 1.0F, 3.5F);
        const float cornerRadius = safeCornerRadius(style.cornerRadius, node.size);
        const glm::vec4 border = selected ? style.selectionColor : style.borderColor;

        std::vector<ColorVertex> fillVertices;
        fillVertices.reserve(512U);

        const std::array<glm::vec4, 4> borderColors = {
            opaque(brighten(border, selected ? 0.18F : 0.10F)),
            opaque(brighten(border, selected ? 0.12F : 0.06F)),
            opaque(border),
            opaque(border),
        };
        appendConvexPolygon(fillVertices, roundedRectPoints(node.position, node.size, cornerRadius, 8), borderColors,
                            node.position, node.position + node.size);

        const glm::vec2 innerPos = node.position + glm::vec2(borderThickness);
        const glm::vec2 innerSize = node.size - glm::vec2(borderThickness * 2.0F);
        const float innerRadius = std::max(cornerRadius - borderThickness, 2.0F);

        const glm::vec4 bodyTop = opaque(brighten(style.bodyColor, 0.05F));
        const glm::vec4 bodyBottom = opaque(style.bodyColor);
        const std::array<glm::vec4, 4> bodyColors = {bodyTop, bodyTop, bodyBottom, bodyBottom};
        appendConvexPolygon(fillVertices, roundedRectPoints(innerPos, innerSize, innerRadius, 8), bodyColors, innerPos,
                            innerPos + innerSize);

        glBindBuffer(GL_ARRAY_BUFFER, m_fillVbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(fillVertices.size() * sizeof(ColorVertex)),
                     fillVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(fillVertices.size()));
    }

    glBindVertexArray(m_lineVao);
    for (const Scene::NodeRenderData& node : nodes) {
        const NodeRenderStyle style = styleResolver ? styleResolver(node.typeId) : defaultStyle(node.typeId);
        const bool selected = isSelected(node.id, selectedNodeIds);
        const float cornerRadius = safeCornerRadius(style.cornerRadius, node.size);
        const glm::vec4 border = selected ? brighten(style.selectionColor, 0.08F) : brighten(style.borderColor, 0.04F);
        const std::vector<glm::vec2> outline = roundedRectPoints(node.position, node.size, cornerRadius, 8);
        std::vector<ColorVertex> outlineVertices;
        outlineVertices.reserve(outline.size());
        for (const glm::vec2& point : outline)
            outlineVertices.push_back({point.x, point.y, border.r, border.g, border.b, border.a});

        glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(outlineVertices.size() * sizeof(ColorVertex)),
                     outlineVertices.data(), GL_DYNAMIC_DRAW);
        glLineWidth(selected ? style.borderThickness + 2.0F : style.borderThickness + 0.45F);
        glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(outlineVertices.size()));
    }

    glBindVertexArray(0);
}

void NodeRenderer::renderSelectionBorders(const std::vector<Scene::NodeRenderData>& nodes,
                                          const Camera::Camera2D& camera,
                                          const std::vector<NodeId>& selectedNodeIds,
                                          const StyleResolver& styleResolver) const {
    if (nodes.empty() || selectedNodeIds.empty()) {
        return;
    }

    glUseProgram(m_program);

    const glm::mat4 vp = camera.viewProjection();
    const int vpLoc = glGetUniformLocation(m_program, "uVP");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, &vp[0][0]);

    glBindVertexArray(m_fillVao);
    for (const Scene::NodeRenderData& node : nodes) {
        if (!isSelected(node.id, selectedNodeIds))
            continue;

        const NodeRenderStyle style = styleResolver ? styleResolver(node.typeId) : defaultStyle(node.typeId);
        const float borderThickness = std::clamp(style.borderThickness + 1.1F, 1.0F, 3.5F);
        const float cornerRadius = safeCornerRadius(style.cornerRadius, node.size);

        std::vector<ColorVertex> fillVertices;
        fillVertices.reserve(256U);

        const std::array<glm::vec4, 4> borderColors = {
            opaque(brighten(style.selectionColor, 0.18F)),
            opaque(brighten(style.selectionColor, 0.12F)),
            opaque(style.selectionColor),
            opaque(style.selectionColor),
        };
        const glm::vec2 innerPos = node.position + glm::vec2(borderThickness);
        const glm::vec2 innerSize = node.size - glm::vec2(borderThickness * 2.0F);
        const float innerRadius = std::max(cornerRadius - borderThickness, 2.0F);
        appendBorderRing(fillVertices, roundedRectPoints(node.position, node.size, cornerRadius, 8),
                         roundedRectPoints(innerPos, innerSize, innerRadius, 8), borderColors, node.position,
                         node.position + node.size);

        glBindBuffer(GL_ARRAY_BUFFER, m_fillVbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(fillVertices.size() * sizeof(ColorVertex)),
                     fillVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(fillVertices.size()));
    }

    glBindVertexArray(0);
}

} // namespace ScopeCanvas::Render

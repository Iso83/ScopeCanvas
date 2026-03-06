#include "Render/EdgeRenderer.h"

#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

#include <cmath>

EdgeRenderer::EdgeRenderer() : m_vao(0), m_vbo(0) {}

EdgeRenderer::~EdgeRenderer() {
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

bool EdgeRenderer::init(const char* vertexShaderPath, const char* fragmentShaderPath) {
    if (!m_shader.loadFromFiles(vertexShaderPath, fragmentShaderPath)) {
        return false;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 64, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

    glBindVertexArray(0);
    return true;
}

void EdgeRenderer::renderEdges(const DiagramModel& model, const glm::mat4& viewProjection) const {
    constexpr int kSegments = 20;

    for (const Edge& edge : model.edges()) {
        const Node* fromNode = model.findNode(edge.fromNode);
        const Node* toNode = model.findNode(edge.toNode);
        const Connector* fromConnector = model.findConnector(edge.fromNode, edge.fromConnector);
        const Connector* toConnector = model.findConnector(edge.toNode, edge.toConnector);

        if (fromNode == nullptr || toNode == nullptr || fromConnector == nullptr || toConnector == nullptr) {
            continue;
        }

        const glm::vec2 p0 = connectorWorldPosition(*fromNode, *fromConnector);
        const glm::vec2 p3 = connectorWorldPosition(*toNode, *toConnector);
        const float dx = std::abs(p3.x - p0.x);
        const glm::vec2 p1 = p0 + glm::vec2(dx * 0.5f, 0.0f);
        const glm::vec2 p2 = p3 - glm::vec2(dx * 0.5f, 0.0f);

        const std::vector<glm::vec2> points = sampleBezier(p0, p1, p2, p3, kSegments);
        renderPolyline(points, viewProjection, glm::vec3(0.8f, 0.8f, 0.85f), 2.0f);
    }
}

void EdgeRenderer::renderConnectors(const std::vector<Node>& nodes, const glm::mat4& viewProjection) const {
    std::vector<glm::vec2> points;
    points.reserve(nodes.size() * 4);

    for (const Node& node : nodes) {
        for (const Connector& connector : node.connectors) {
            points.push_back(connectorWorldPosition(node, connector));
        }
    }

    if (points.empty()) {
        return;
    }

    m_shader.use();
    const GLuint programId = m_shader.id();

    const GLint vpLoc = glGetUniformLocation(programId, "uViewProjection");
    const GLint colorLoc = glGetUniformLocation(programId, "uColor");

    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform3f(colorLoc, 0.9f, 0.9f, 0.95f);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(points.size() * sizeof(glm::vec2)),
                 points.data(),
                 GL_DYNAMIC_DRAW);

    glPointSize(6.0f);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));

    glBindVertexArray(0);
}

void EdgeRenderer::renderPreviewEdge(const DiagramModel& model,
                                     uint32_t startNodeId,
                                     uint32_t startConnectorId,
                                     const glm::vec2& previewPosition,
                                     const glm::mat4& viewProjection) const {
    const Node* fromNode = model.findNode(startNodeId);
    const Connector* fromConnector = model.findConnector(startNodeId, startConnectorId);
    if (fromNode == nullptr || fromConnector == nullptr) {
        return;
    }

    const glm::vec2 p0 = connectorWorldPosition(*fromNode, *fromConnector);
    const glm::vec2 p3 = previewPosition;
    const float dx = std::abs(p3.x - p0.x);
    const glm::vec2 p1 = p0 + glm::vec2(dx * 0.5f, 0.0f);
    const glm::vec2 p2 = p3 - glm::vec2(dx * 0.5f, 0.0f);

    const std::vector<glm::vec2> points = sampleBezier(p0, p1, p2, p3, 20);
    renderPolyline(points, viewProjection, glm::vec3(0.85f, 0.85f, 0.9f), 2.0f);
}

std::vector<glm::vec2> EdgeRenderer::sampleBezier(const glm::vec2& p0,
                                                  const glm::vec2& p1,
                                                  const glm::vec2& p2,
                                                  const glm::vec2& p3,
                                                  int segments) {
    std::vector<glm::vec2> points;
    points.reserve(static_cast<size_t>(segments) + 1);

    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const float omt = 1.0f - t;
        const glm::vec2 point =
            omt * omt * omt * p0 +
            3.0f * omt * omt * t * p1 +
            3.0f * omt * t * t * p2 +
            t * t * t * p3;
        points.push_back(point);
    }

    return points;
}

void EdgeRenderer::renderPolyline(const std::vector<glm::vec2>& points,
                                  const glm::mat4& viewProjection,
                                  const glm::vec3& color,
                                  float thickness) const {
    if (points.empty()) {
        return;
    }

    m_shader.use();
    const GLuint programId = m_shader.id();

    const GLint vpLoc = glGetUniformLocation(programId, "uViewProjection");
    const GLint colorLoc = glGetUniformLocation(programId, "uColor");

    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform3f(colorLoc, color.x, color.y, color.z);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(points.size() * sizeof(glm::vec2)),
                 points.data(),
                 GL_DYNAMIC_DRAW);

    glLineWidth(thickness);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(points.size()));

    glBindVertexArray(0);
}

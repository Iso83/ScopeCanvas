#include "Render/EdgeRenderer.h"

#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

#include <cmath>
#include <unordered_set>

EdgeRenderer::EdgeRenderer() : m_vao(0), m_vbo(0), m_vboCapacityBytes(0) {}

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
    m_vboCapacityBytes = sizeof(glm::vec2) * 256;
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_vboCapacityBytes), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

    glBindVertexArray(0);
    return true;
}

void EdgeRenderer::renderEdges(const DiagramModel& model,
                               const glm::mat4& viewProjection,
                               uint32_t hoveredEdgeId) {
    std::unordered_set<uint32_t> hiddenNodeIds;
    for (const Group& group : model.groups()) {
        if (!group.collapsed) {
            continue;
        }

        for (uint32_t childNodeId : group.children) {
            hiddenNodeIds.insert(childNodeId);
        }
    }

    for (const Edge& edge : model.edges()) {
        if (hiddenNodeIds.find(edge.fromNode) != hiddenNodeIds.end() ||
            hiddenNodeIds.find(edge.toNode) != hiddenNodeIds.end()) {
            continue;
        }
        if (edge.route.points.size() < 2) {
            continue;
        }

        glm::vec3 color(0.8f, 0.8f, 0.85f);
        if (edge.selected) {
            color = glm::vec3(1.0f, 0.8f, 0.3f);
        }
        if (edge.id == hoveredEdgeId) {
            color = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        for (size_t i = 0; i + 1 < edge.route.points.size(); ++i) {
            renderLineSegment(edge.route.points[i], edge.route.points[i + 1], viewProjection, color, 2.0f);
        }
    }
}

void EdgeRenderer::renderConnectors(const std::vector<Node>& nodes,
                                    const glm::mat4& viewProjection,
                                    uint32_t hoveredConnectorId) {
    for (const Node& node : nodes) {
        for (const Connector& connector : node.connectors) {
            m_scratchPoints.clear();
            m_scratchPoints.push_back(connectorWorldPosition(node, connector));

            const glm::vec3 color = connector.id == hoveredConnectorId
                ? glm::vec3(1.0f, 1.0f, 1.0f)
                : glm::vec3(0.9f, 0.9f, 0.95f);

            m_shader.use();
            const GLuint programId = m_shader.id();

            const GLint vpLoc = glGetUniformLocation(programId, "uViewProjection");
            const GLint colorLoc = glGetUniformLocation(programId, "uColor");

            glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));
            glUniform3f(colorLoc, color.x, color.y, color.z);

            glBindVertexArray(m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            ensureBufferCapacity(m_scratchPoints.size());
            glBufferSubData(GL_ARRAY_BUFFER,
                            0,
                            static_cast<GLsizeiptr>(m_scratchPoints.size() * sizeof(glm::vec2)),
                            m_scratchPoints.data());

            glPointSize(6.0f);
            glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_scratchPoints.size()));

            glBindVertexArray(0);
        }
    }
}

void EdgeRenderer::renderPreviewEdge(const DiagramModel& model,
                                     uint32_t startNodeId,
                                     uint32_t startConnectorId,
                                     const glm::vec2& previewPosition,
                                     const glm::mat4& viewProjection) {
    for (const Group& group : model.groups()) {
        if (!group.collapsed) {
            continue;
        }

        if (std::find(group.children.begin(), group.children.end(), startNodeId) != group.children.end()) {
            return;
        }
    }

    const Node* fromNode = model.findNode(startNodeId);
    const Connector* fromConnector = model.findConnector(startNodeId, startConnectorId);
    if (fromNode == nullptr || fromConnector == nullptr) {
        return;
    }

    const glm::vec2 p0 = connectorWorldPosition(*fromNode, *fromConnector);
    const glm::vec2 p3 = previewPosition;

    glm::vec2 p1(0.0f);
    glm::vec2 p2(0.0f);
    computeBezierControls(p0, p3, p1, p2);

    m_scratchPoints.clear();
    appendBezierSamples(m_scratchPoints, p0, p1, p2, p3, 20);
    renderPolyline(m_scratchPoints, viewProjection, glm::vec3(0.85f, 0.85f, 0.9f), 2.0f);
}

void EdgeRenderer::appendBezierSamples(std::vector<glm::vec2>& points,
                                       const glm::vec2& p0,
                                       const glm::vec2& p1,
                                       const glm::vec2& p2,
                                       const glm::vec2& p3,
                                       int segments) {
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
}

void EdgeRenderer::computeBezierControls(const glm::vec2& p0,
                                         const glm::vec2& p3,
                                         glm::vec2& p1,
                                         glm::vec2& p2) {
    const float dx = std::abs(p3.x - p0.x);
    const float controlDistance = glm::max(80.0f, dx * 0.5f);
    p1 = p0 + glm::vec2(controlDistance, 0.0f);
    p2 = p3 - glm::vec2(controlDistance, 0.0f);
}

void EdgeRenderer::renderLineSegment(const glm::vec2& start,
                                     const glm::vec2& end,
                                     const glm::mat4& viewProjection,
                                     const glm::vec3& color,
                                     float thickness) {
    m_scratchPoints.clear();
    m_scratchPoints.push_back(start);
    m_scratchPoints.push_back(end);
    renderPolyline(m_scratchPoints, viewProjection, color, thickness);
}

void EdgeRenderer::ensureBufferCapacity(size_t pointCount) {
    const size_t requiredBytes = pointCount * sizeof(glm::vec2);
    if (requiredBytes <= m_vboCapacityBytes) {
        return;
    }

    m_vboCapacityBytes = requiredBytes;
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_vboCapacityBytes), nullptr, GL_DYNAMIC_DRAW);
}

void EdgeRenderer::renderPolyline(const std::vector<glm::vec2>& points,
                                  const glm::mat4& viewProjection,
                                  const glm::vec3& color,
                                  float thickness) {
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
    ensureBufferCapacity(points.size());
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    static_cast<GLsizeiptr>(points.size() * sizeof(glm::vec2)),
                    points.data());

    glLineWidth(thickness);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(points.size()));

    glBindVertexArray(0);
}

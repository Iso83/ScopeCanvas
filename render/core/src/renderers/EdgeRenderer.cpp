#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/render/renderers/EdgeRenderer.h>
#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ScopeCanvas::Render::Renderers {
EdgeRenderer::EdgeRenderer() = default;

EdgeRenderer::~EdgeRenderer() {
    destroy();
}

bool EdgeRenderer::init() {
    if (!m_shader.load(GL::EdgeVertex, GL::EdgeFragment)) {
        return false;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(sizeof(glm::vec2)), nullptr);
    glBindVertexArray(0);
    return true;
}

void EdgeRenderer::shutdown() {
    destroy();
}

void EdgeRenderer::render(const std::vector<Scene::EdgeRenderData>& edges, const Camera::Camera2D& camera,
                          Core::CanvasEdgeId hoveredEdgeId, Core::CanvasEdgeId selectedEdgeId) const {
    for (const Scene::EdgeRenderData& edge : edges) {
        if (edge.points.size() < 2U) {
            continue;
        }
        Routing::EdgeRoute route{};
        route.edgeId = edge.edgeId;
        route.points = edge.points;

        glm::vec3 color{0.72F, 0.75F, 0.83F};
        float thickness = 2.0F;
        if (selectedEdgeId.isValid() && edge.edgeId == selectedEdgeId) {
            color = {0.98F, 0.76F, 0.33F};
            thickness = 3.0F;
        }
        if (hoveredEdgeId.isValid() && edge.edgeId == hoveredEdgeId) {
            color = {0.96F, 0.97F, 1.0F};
            thickness = std::max(thickness, 2.5F);
        }

        renderPolyline(buildEdgeGeometry(route, 18), camera, color, thickness);
    }
}

void EdgeRenderer::renderConnectors(const std::vector<Scene::ConnectorAnchorRenderData>& connectors,
                                    const Camera::Camera2D& camera, Core::CanvasConnectorId hoveredConnectorId,
                                    Core::CanvasConnectorId activeConnectorId) const {
    if (connectors.empty()) {
        return;
    }

    m_shader.use();
    const GLuint programId = m_shader.id();
    const GLint vpLoc = glGetUniformLocation(programId, "uViewProjection");
    const GLint colorLoc = glGetUniformLocation(programId, "uColor");
    const glm::mat4 viewProjection = camera.viewProjection();

    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    for (const Scene::ConnectorAnchorRenderData& connector : connectors) {
        const bool hovered = hoveredConnectorId.isValid() && connector.connectorId == hoveredConnectorId;
        const bool active = activeConnectorId.isValid() && connector.connectorId == activeConnectorId;
        const float pointSize = active ? 11.0F : hovered ? 9.5F : 7.0F;
        const glm::vec3 color = active ? glm::vec3(1.0F, 0.84F, 0.30F)
                                       : hovered ? glm::vec3(1.0F, 1.0F, 1.0F)
                                                 : connector.output ? glm::vec3(0.95F, 0.75F, 0.28F)
                                                                    : glm::vec3(0.82F, 0.85F, 0.94F);
        glUniform3f(colorLoc, color.x, color.y, color.z);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(glm::vec2)), &connector.anchor, GL_DYNAMIC_DRAW);
        glPointSize(pointSize);
        glDrawArrays(GL_POINTS, 0, 1);
    }

    glBindVertexArray(0);
}

void EdgeRenderer::renderPreviewEdge(const glm::vec2& start, const glm::vec2& end, const glm::vec2& startNormal,
                                     const Camera::Camera2D& camera) const {
    Routing::EdgeRoute route{};
    route.points.push_back(start);

    glm::vec2 normal = startNormal;
    if (glm::dot(normal, normal) <= 0.0001F) {
        normal = {1.0F, 0.0F};
    } else {
        normal = glm::normalize(normal);
    }

    const glm::vec2 delta = end - start;
    const float forwardDistance = glm::dot(delta, normal);
    if (forwardDistance < 48.0F) {
        const float escapeDistance = std::clamp(std::abs(delta.x) * 0.35F + 28.0F, 48.0F, 96.0F);
        const glm::vec2 breakout = start + normal * escapeDistance;
        route.points.push_back(breakout);
        if (std::abs(end.y - breakout.y) > 6.0F) {
            route.points.push_back({breakout.x, end.y});
        }
    }

    route.points.push_back(end);
    renderPolyline(buildEdgeGeometry(route, 20), camera, {0.92F, 0.94F, 1.0F}, 2.0F);
}

std::vector<glm::vec2> EdgeRenderer::buildEdgeGeometry(const Routing::EdgeRoute& route, int segmentsPerCurve) const {
    if (route.points.size() < 2) {
        return route.points;
    }
    if (route.preferStraightSegments) {
        return route.points;
    }

    const int segments = std::max(segmentsPerCurve, 1);
    if (route.points.size() == 2U) {
        std::vector<glm::vec2> geometry;
        geometry.reserve(route.points.size() * static_cast<std::size_t>(segments));

        for (std::size_t i = 0; i + 1 < route.points.size(); ++i) {
            const glm::vec2 p0 = route.points[i];
            const glm::vec2 p3 = route.points[i + 1];

            glm::vec2 p1{};
            glm::vec2 p2{};
            computeBezierControls(p0, p3, p1, p2);

            appendBezierSamples(geometry, p0, p1, p2, p3, segments);
        }

        return geometry;
    }

    std::vector<glm::vec2> geometry;
    geometry.reserve(route.points.size() * static_cast<std::size_t>(segments));
    geometry.push_back(route.points.front());

    for (std::size_t i = 1; i + 1 < route.points.size(); ++i) {
        const glm::vec2 previous = route.points[i - 1];
        const glm::vec2 current = route.points[i];
        const glm::vec2 next = route.points[i + 1];

        const glm::vec2 incoming = current - previous;
        const glm::vec2 outgoing = next - current;
        const float incomingLength = glm::length(incoming);
        const float outgoingLength = glm::length(outgoing);
        if (incomingLength <= 0.001F || outgoingLength <= 0.001F) {
            if (glm::distance(geometry.back(), current) > 0.001F) {
                geometry.push_back(current);
            }
            continue;
        }

        const glm::vec2 incomingDir = incoming / incomingLength;
        const glm::vec2 outgoingDir = outgoing / outgoingLength;
        const float alignment = glm::dot(incomingDir, outgoingDir);
        if (std::abs(alignment) > 0.999F) {
            if (glm::distance(geometry.back(), current) > 0.001F) {
                geometry.push_back(current);
            }
            continue;
        }

        const float cornerRadius = std::min({24.0F, incomingLength * 0.35F, outgoingLength * 0.35F});
        const glm::vec2 cornerStart = current - incomingDir * cornerRadius;
        const glm::vec2 cornerEnd = current + outgoingDir * cornerRadius;
        if (glm::distance(geometry.back(), cornerStart) > 0.001F) {
            geometry.push_back(cornerStart);
        }
        appendQuadraticSamples(geometry, cornerStart, current, cornerEnd, std::max(segments / 2, 6));
    }

    if (glm::distance(geometry.back(), route.points.back()) > 0.001F) {
        geometry.push_back(route.points.back());
    }
    return geometry;
}

void EdgeRenderer::computeBezierControls(const glm::vec2& p0, const glm::vec2& p3, glm::vec2& p1, glm::vec2& p2) {
    const float dx = std::abs(p3.x - p0.x);
    const float controlDistance = std::max(80.0F, dx * 0.5F);
    p1 = {p0.x + controlDistance, p0.y};
    p2 = {p3.x - controlDistance, p3.y};
}

void EdgeRenderer::appendBezierSamples(std::vector<glm::vec2>& points, const glm::vec2& p0, const glm::vec2& p1,
                                       const glm::vec2& p2, const glm::vec2& p3, int segments) {
    if (!points.empty()) {
        points.pop_back();
    }

    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const float omt = 1.0F - t;

        const float x =
            omt * omt * omt * p0.x + 3.0F * omt * omt * t * p1.x + 3.0F * omt * t * t * p2.x + t * t * t * p3.x;
        const float y =
            omt * omt * omt * p0.y + 3.0F * omt * omt * t * p1.y + 3.0F * omt * t * t * p2.y + t * t * t * p3.y;

        points.push_back({x, y});
    }
}

void EdgeRenderer::appendQuadraticSamples(std::vector<glm::vec2>& points, const glm::vec2& p0, const glm::vec2& p1,
                                          const glm::vec2& p2, int segments) {
    if (!points.empty()) {
        points.pop_back();
    }

    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const float omt = 1.0F - t;
        const glm::vec2 point = omt * omt * p0 + 2.0F * omt * t * p1 + t * t * p2;
        points.push_back(point);
    }
}

void EdgeRenderer::renderPolyline(const std::vector<glm::vec2>& points, const Camera::Camera2D& camera,
                                  const glm::vec3& color, float thickness, GLenum primitive) const {
    if (points.size() < 2U && primitive != GL_POINTS) {
        return;
    }

    m_shader.use();

    const GLuint programId = m_shader.id();
    const GLint vpLoc = glGetUniformLocation(programId, "uViewProjection");
    const GLint colorLoc = glGetUniformLocation(programId, "uColor");
    const glm::mat4 viewProjection = camera.viewProjection();

    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform3f(colorLoc, color.x, color.y, color.z);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(points.size() * sizeof(glm::vec2)), points.data(),
                 GL_DYNAMIC_DRAW);
    glLineWidth(thickness);
    glDrawArrays(primitive, 0, static_cast<GLsizei>(points.size()));
    glBindVertexArray(0);
}

void EdgeRenderer::destroy() {
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}
} // namespace ScopeCanvas::Render::Renderers

#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/render/renderers/EdgeRenderer.h>
#include <algorithm>
#include <cmath>
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

void EdgeRenderer::render(const std::vector<Scene::EdgeRenderData>& edges, const Camera::Camera2D& camera) const {
    for (const Scene::EdgeRenderData& edge : edges) {
        if (edge.points.size() < 2U) {
            continue;
        }
        Routing::EdgeRoute route{};
        route.edgeId = edge.edgeId;
        route.points = edge.points;
        renderPolyline(buildEdgeGeometry(route, 16), camera, {0.84F, 0.86F, 0.92F}, 2.0F);
    }
}

void EdgeRenderer::renderConnectors(const std::vector<Scene::ConnectorAnchorRenderData>& connectors,
                                    const Camera::Camera2D& camera) const {
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

    glPointSize(7.0F);
    for (const Scene::ConnectorAnchorRenderData& connector : connectors) {
        const glm::vec2 point = connector.anchor;
        const glm::vec3 color = connector.output ? glm::vec3(1.0F, 0.83F, 0.33F) : glm::vec3(0.86F, 0.88F, 0.96F);
        glUniform3f(colorLoc, color.x, color.y, color.z);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(glm::vec2)), &point, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_POINTS, 0, 1);
    }

    glBindVertexArray(0);
}

std::vector<glm::vec2> EdgeRenderer::buildEdgeGeometry(const Routing::EdgeRoute& route, int segmentsPerCurve) const {
    if (route.points.size() < 2) {
        return route.points;
    }

    const int segments = std::max(segmentsPerCurve, 1);
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

void EdgeRenderer::renderPolyline(const std::vector<glm::vec2>& points, const Camera::Camera2D& camera,
                                  const glm::vec3& color, float thickness) const {
    if (points.size() < 2U) {
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
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(points.size()));
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

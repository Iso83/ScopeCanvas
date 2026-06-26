#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/render/gl/Shader.h>
#include <ScopeCanvas/render/scene/RenderScene.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

namespace ScopeCanvas::Routing {
struct EdgeRoute;
}

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Render {
class EdgeRenderer {
  private:
    GL::Shader m_shader{};
    GLuint m_vao{0};
    GLuint m_vbo{0};

  public:
    EdgeRenderer();
    ~EdgeRenderer();

    EdgeRenderer(const EdgeRenderer&) = delete;
    EdgeRenderer& operator=(const EdgeRenderer&) = delete;

    bool init();
    void shutdown();

    void render(const std::vector<Scene::EdgeRenderData>& edges, const Camera::Camera2D& camera,
                Core::Ids::EdgeId hoveredEdgeId = {}, Core::Ids::EdgeId selectedEdgeId = {}) const;
    void renderConnectors(const std::vector<Scene::ConnectorAnchorRenderData>& connectors,
                          const Camera::Camera2D& camera, Core::Ids::ConnectorId hoveredConnectorId = {},
                          Core::Ids::ConnectorId activeConnectorId = {}) const;
    void renderPreviewEdge(const glm::vec2& start, const glm::vec2& end, const glm::vec2& startNormal,
                           const Camera::Camera2D& camera) const;

    [[nodiscard]] static std::vector<glm::vec2> buildEdgeGeometry(const Routing::EdgeRoute& route,
                                                                  int segmentsPerCurve = 20);

  private:
    static void computeBezierControls(const glm::vec2& p0, const glm::vec2& p3, glm::vec2& p1, glm::vec2& p2);
    static void appendBezierSamples(std::vector<glm::vec2>& points, const glm::vec2& p0, const glm::vec2& p1,
                                    const glm::vec2& p2, const glm::vec2& p3, int segments);
    static void appendQuadraticSamples(std::vector<glm::vec2>& points, const glm::vec2& p0, const glm::vec2& p1,
                                       const glm::vec2& p2, int segments);
    void renderPolyline(const std::vector<glm::vec2>& points, const Camera::Camera2D& camera, const glm::vec3& color,
                        float thickness, GLenum primitive = GL_LINE_STRIP) const;
    void destroy();
};
} // namespace ScopeCanvas::Render

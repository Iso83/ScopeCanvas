#pragma once

#include <ScopeCanvas/render/gl/Shader.h>
#include <ScopeCanvas/render/scene/RenderScene.h>
#include <ScopeCanvas/routing/EdgeRoute.h>
#include <glm/vec3.hpp>
#include <vector>

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Render::Renderers {
class EdgeRenderer {
  public:
    EdgeRenderer();
    ~EdgeRenderer();

    EdgeRenderer(const EdgeRenderer&) = delete;
    EdgeRenderer& operator=(const EdgeRenderer&) = delete;

    bool init();
    void shutdown();

    void render(const std::vector<Scene::EdgeRenderData>& edges, const Camera::Camera2D& camera) const;
    void renderConnectors(const std::vector<Scene::ConnectorAnchorRenderData>& connectors,
                          const Camera::Camera2D& camera) const;

    [[nodiscard]] std::vector<glm::vec2> buildEdgeGeometry(const Routing::EdgeRoute& route,
                                                           int segmentsPerCurve = 20) const;

  private:
    static void computeBezierControls(const glm::vec2& p0, const glm::vec2& p3, glm::vec2& p1, glm::vec2& p2);
    static void appendBezierSamples(std::vector<glm::vec2>& points, const glm::vec2& p0, const glm::vec2& p1,
                                    const glm::vec2& p2, const glm::vec2& p3, int segments);
    void renderPolyline(const std::vector<glm::vec2>& points, const Camera::Camera2D& camera, const glm::vec3& color,
                        float thickness) const;
    void destroy();

    GL::Shader m_shader{};
    GLuint m_vao{0};
    GLuint m_vbo{0};
};
} // namespace ScopeCanvas::Render::Renderers

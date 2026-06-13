#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/render/renderers/EdgeRenderer.h>
#include <ScopeCanvas/render/renderers/GridRenderer.h>
#include <ScopeCanvas/render/renderers/NodeRenderer.h>
#include <ScopeCanvas/render/renderers/SelectionRectRenderer.h>
#include <ScopeCanvas/render/scene/SceneBuilder.h>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Routing {
class IGraphView;
struct EdgeRoute;
} // namespace ScopeCanvas::Routing

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Render {

struct CanvasRenderOptions {
    bool showGrid{true};
    bool showEdges{true};
    bool showNodes{true};
    bool showConnectors{true};
    bool showDebug{false};
    float gridSize{32.0f};

    std::vector<Core::Ids::NodeId> selectedNodeIds{};
    Core::Ids::EdgeId hoveredEdgeId{};
    Core::Ids::EdgeId selectedEdgeId{};
    Core::Ids::ConnectorId hoveredConnectorId{};
    Core::Ids::ConnectorId activeConnectorId{};
    bool previewEdgeActive{false};
    glm::vec2 previewEdgeStart{};
    glm::vec2 previewEdgeEnd{};
    glm::vec2 previewEdgeStartNormal{1.0F, 0.0F};

    bool selectionRectActive{false};
    glm::vec2 selectionRectStart{};
    glm::vec2 selectionRectEnd{};

    Renderers::NodeRenderer::StyleResolver nodeStyleResolver{};
    Renderers::NodeRenderer::TitleResolver nodeTitleResolver{};
    Renderers::NodeRenderer::IconResolver nodeIconResolver{};
};

class CanvasRenderer {
  private:
    Renderers::GridRenderer m_grid{};
    Renderers::NodeRenderer m_nodes{};
    Renderers::EdgeRenderer m_edges{};
    Renderers::SelectionRectRenderer m_selection{};
    Scene::SceneBuilder m_sceneBuilder{};

  public:
    bool init();
    void shutdown();

    void render(const Routing::IGraphView& document, const std::vector<Routing::EdgeRoute>& routes,
                const Camera::Camera2D& camera, const CanvasRenderOptions& options) const;
    void renderSelectionRect(const Camera::Camera2D& camera, const glm::vec2& start, const glm::vec2& end) const;
};

} // namespace ScopeCanvas::Render

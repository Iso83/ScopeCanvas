#pragma once

#include <ScopeCanvas/engine/core/ids/CanvasIds.h>
#include <ScopeCanvas/engine/render/EdgeRenderer.h>
#include <ScopeCanvas/engine/render/GridRenderer.h>
#include <ScopeCanvas/engine/render/NodeRenderer.h>
#include <ScopeCanvas/engine/render/SelectionRectRenderer.h>
#include <ScopeCanvas/engine/render/scene/SceneBuilder.h>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Engine::Routing {
class IGraphView;
struct EdgeRoute;
} // namespace ScopeCanvas::Engine::Routing

namespace ScopeCanvas::Engine::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Engine::Render {

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

    NodeRenderer::StyleResolver nodeStyleResolver{};
};

class CanvasRenderer {
private:
    GridRenderer m_grid{};
    NodeRenderer m_nodes{};
    EdgeRenderer m_edges{};
    SelectionRectRenderer m_selection{};
    Scene::SceneBuilder m_sceneBuilder{};

public:
    bool init();
    void shutdown();

    void render(const Routing::IGraphView& document, const std::vector<Routing::EdgeRoute>& routes,
                const Camera::Camera2D& camera, const CanvasRenderOptions& options) const;
    void renderNodeSelectionBorders(const Routing::IGraphView& document, const std::vector<Routing::EdgeRoute>& routes,
                                    const Camera::Camera2D& camera, const CanvasRenderOptions& options) const;
    void renderSelectionRect(const Camera::Camera2D& camera, const glm::vec2& start, const glm::vec2& end) const;
};

} // namespace ScopeCanvas::Engine::Render

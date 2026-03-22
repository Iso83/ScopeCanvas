#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/render/renderers/EdgeRenderer.h>
#include <ScopeCanvas/render/renderers/GridRenderer.h>
#include <ScopeCanvas/render/renderers/NodeRenderer.h>
#include <ScopeCanvas/render/renderers/SelectionRectRenderer.h>
#include <ScopeCanvas/render/scene/SceneBuilder.h>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Core {
class GraphDocument;
}

namespace ScopeCanvas::Routing {
struct EdgeRoute;
}

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Render {

struct CanvasRenderOptions {
    bool showGrid{true};
    bool showEdges{true};
    bool showNodes{true};
    bool showConnectors{true};
    float gridSize{32.0f};

    Core::CanvasNodeId selectedNodeId{};
    bool selectionRectActive{false};
    glm::vec2 selectionRectStart{};
    glm::vec2 selectionRectEnd{};
};

class CanvasRenderer {
  public:
    bool init();
    void shutdown();

    void render(const Core::GraphDocument& document, const std::vector<Routing::EdgeRoute>& routes,
                const Camera::Camera2D& camera, const CanvasRenderOptions& options) const;

  private:
    Renderers::GridRenderer m_grid{};
    Renderers::NodeRenderer m_nodes{};
    Renderers::EdgeRenderer m_edges{};
    Renderers::SelectionRectRenderer m_selection{};
    Scene::SceneBuilder m_sceneBuilder{};
};

} // namespace ScopeCanvas::Render

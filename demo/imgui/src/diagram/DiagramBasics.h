#pragma once

#include <ScopeCanvas/core/GraphDocument.h>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/routing/EdgeRouter.h>
#include <vector>

namespace ScopeCanvas::Studio {
struct GridSettings {
    bool enabled{true};
    bool snapEnabled{true};
    float cellSize{32.0F};
};

struct ViewState {
    float cameraX{0.0F};
    float cameraY{0.0F};
    float zoom{1.0F};
};

class DiagramBasics {
  public:
    DiagramBasics();

    Core::GraphDocument& model();
    const Core::GraphDocument& model() const;

    GridSettings& gridSettings();
    const GridSettings& gridSettings() const;

    std::vector<Core::CanvasNodeId>& nodeIds();
    const std::vector<Core::CanvasNodeId>& nodeIds() const;

    std::vector<Core::CanvasEdgeId>& edgeIds();
    const std::vector<Core::CanvasEdgeId>& edgeIds() const;

    std::vector<Core::CanvasNodeId>& selectedNodeIds();
    const std::vector<Core::CanvasNodeId>& selectedNodeIds() const;

    Core::CanvasNodeId createNode(Core::NodeTypeId typeId, glm::vec2 position);
    Core::CanvasEdgeId connect(Core::CanvasConnectorId a, Core::CanvasConnectorId b);
    void deleteEdge(Core::CanvasEdgeId edgeId);
    void deleteNode(Core::CanvasNodeId nodeId);

    void clearSelection();
    void setSelection(const std::vector<Core::CanvasNodeId>& nodeIds);
    void setNodeSelected(Core::CanvasNodeId nodeId, bool selected);
    [[nodiscard]] bool isNodeSelected(Core::CanvasNodeId nodeId) const;

    std::vector<Routing::EdgeRoute> routeAllEdges() const;

  private:
    Core::GraphDocument m_model{};
    GridSettings m_grid{};
    std::vector<Core::CanvasNodeId> m_nodeIds{};
    std::vector<Core::CanvasEdgeId> m_edgeIds{};
    std::vector<Core::CanvasNodeId> m_selectedNodeIds{};
};
} // namespace ScopeCanvas::Studio

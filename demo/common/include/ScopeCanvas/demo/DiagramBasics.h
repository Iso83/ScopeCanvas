#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/widget/GraphDocument.h>
#include <vector>

namespace ScopeCanvas::Demo {

struct GridSnapper {
    bool enabled{true};
    bool snapEnabled{true};
    float cellSize{32.0F};
};

class DiagramBasics {
  private:
    Widget::GraphDocument m_model{};
    GridSnapper m_grid{};
    std::vector<Core::Ids::NodeId> m_nodeIds{};
    std::vector<Core::Ids::EdgeId> m_edgeIds{};
    std::vector<Core::Ids::NodeId> m_selectedNodeIds{};

  public:
    DiagramBasics();

    Widget::GraphDocument& model() {
        return m_model;
    }

    const Widget::GraphDocument& model() const {
        return m_model;
    }

    GridSnapper& gridSettings() {
        return m_grid;
    }

    const GridSnapper& gridSettings() const {
        return m_grid;
    }

    std::vector<Core::Ids::NodeId>& nodeIds() {
        return m_nodeIds;
    }

    const std::vector<Core::Ids::NodeId>& nodeIds() const {
        return m_nodeIds;
    }

    std::vector<Core::Ids::EdgeId>& edgeIds() {
        return m_edgeIds;
    }

    std::vector<Core::Ids::NodeId>& selectedNodeIds() {
        return m_selectedNodeIds;
    }

    Core::Ids::NodeId createNode(Core::Ids::NodeTypeId typeId, glm::vec2 position);

    void deleteNode(Core::Ids::NodeId nodeId);

    Core::Ids::EdgeId connect(Core::Ids::ConnectorId a, Core::Ids::ConnectorId b);

    void deleteEdge(Core::Ids::EdgeId edgeId);

    void setNodeSelected(Core::Ids::NodeId nodeId, bool selected);

    void setSelection(const std::vector<Core::Ids::NodeId>& nodeIds) {
        m_selectedNodeIds = nodeIds;
    }

    void clearSelection() {
        m_selectedNodeIds.clear();
    }

    [[nodiscard]] bool isNodeSelected(Core::Ids::NodeId nodeId) const;

    [[nodiscard]] bool canConnect(ScopeCanvas::Core::Ids::ConnectorId a, ScopeCanvas::Core::Ids::ConnectorId b) const;
};

} // namespace ScopeCanvas::Demo
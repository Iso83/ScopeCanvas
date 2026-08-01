#pragma once

#include <ScopeCanvas/engine/core/ids/CanvasIds.h>
#include <ScopeCanvas/widget/GraphDocument.h>
#include <vector>

namespace ScopeCanvas::Demo::Common {

struct GridSnapper {
    bool enabled{true};
    bool snapEnabled{true};
    float cellSize{32.0F};
};

class DiagramBasics {
private:
    Widget::GraphDocument m_model{};
    GridSnapper m_grid{};
    std::vector<Engine::Core::Ids::NodeId> m_nodeIds{};
    std::vector<Engine::Core::Ids::EdgeId> m_edgeIds{};
    std::vector<Engine::Core::Ids::NodeId> m_selectedNodeIds{};

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

    std::vector<Engine::Core::Ids::NodeId>& nodeIds() {
        return m_nodeIds;
    }

    const std::vector<Engine::Core::Ids::NodeId>& nodeIds() const {
        return m_nodeIds;
    }

    std::vector<Engine::Core::Ids::EdgeId>& edgeIds() {
        return m_edgeIds;
    }

    std::vector<Engine::Core::Ids::NodeId>& selectedNodeIds() {
        return m_selectedNodeIds;
    }

    Engine::Core::Ids::NodeId createNode(Engine::Core::Ids::NodeTypeId typeId, glm::vec2 position);

    void deleteNode(Engine::Core::Ids::NodeId nodeId);

    Engine::Core::Ids::EdgeId connect(Engine::Core::Ids::ConnectorId a, Engine::Core::Ids::ConnectorId b);

    void deleteEdge(Engine::Core::Ids::EdgeId edgeId);

    void setNodeSelected(Engine::Core::Ids::NodeId nodeId, bool selected);

    void setSelection(const std::vector<Engine::Core::Ids::NodeId>& nodeIds) {
        m_selectedNodeIds = nodeIds;
    }

    void clearSelection() {
        m_selectedNodeIds.clear();
    }

    [[nodiscard]] bool isNodeSelected(Engine::Core::Ids::NodeId nodeId) const;

    [[nodiscard]] bool canConnect(ScopeCanvas::Engine::Core::Ids::ConnectorId a,
                                  ScopeCanvas::Engine::Core::Ids::ConnectorId b) const;
};

} // namespace ScopeCanvas::Demo::Common

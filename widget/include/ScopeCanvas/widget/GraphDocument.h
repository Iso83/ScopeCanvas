#pragma once

#include <ScopeCanvas/core/Connector.h>
#include <ScopeCanvas/core/Edge.h>
#include <ScopeCanvas/core/Node.h>
#include <ScopeCanvas/core/ids/IdRegistry.h>
#include <ScopeCanvas/routing/IGraphView.h>
#include <algorithm>
#include <vector>

namespace ScopeCanvas::Widget {
template <typename TGraph, typename TGraphId> struct GraphInstanceRegistry {
    std::vector<TGraph> instances{};
    Core::Ids::GraphRegistry<TGraphId> ids{};

  public:
    TGraph* get(TGraphId id) {
        const auto it =
            std::find_if(instances.begin(), instances.end(), [id](const TGraph& item) { return item.id == id; });
        return it == instances.end() ? nullptr : &(*it);
    }

    const TGraph* get(TGraphId id) const {
        const auto it =
            std::find_if(instances.begin(), instances.end(), [id](const TGraph& value) { return value.id == id; });
        return it == instances.end() ? nullptr : &(*it);
    }

    void erase(TGraphId id) {
        instances.erase(
            std::remove_if(instances.begin(), instances.end(), [id](const TGraph& value) { return value.id == id; }),
            instances.end());
        ids.remove(id);
    }
};

class GraphDocument : public Routing::IGraphView {
  private:
    GraphInstanceRegistry<Core::Node, Core::Ids::NodeId> m_nodes{};
    GraphInstanceRegistry<Core::Connector, Core::Ids::ConnectorId> m_connectors{};
    GraphInstanceRegistry<Core::Edge, Core::Ids::EdgeId> m_edges{};

  public:
    GraphDocument() = default;

    Core::Ids::NodeId createNode(Core::Ids::NodeTypeId typeId);
    void removeNode(Core::Ids::NodeId nodeId);
    Core::Node* getNode(Core::Ids::NodeId nodeId) {
        return m_nodes.get(nodeId);
    }
    const Core::Node* getNode(Core::Ids::NodeId nodeId) const {
        return m_nodes.get(nodeId);
    }

    Core::Connector* getConnector(Core::Ids::ConnectorId connectorId) {
        return m_connectors.get(connectorId);
    }
    const Core::Connector* getConnector(Core::Ids::ConnectorId connectorId) const {
        return m_connectors.get(connectorId);
    }
    glm::vec2 connectorWorld(const Core::Node& node, std::size_t index) const;

    Core::Ids::EdgeId connect(Core::Ids::ConnectorId a, Core::Ids::ConnectorId b);
    void disconnect(Core::Ids::EdgeId edgeId);
    Core::Edge* getEdge(Core::Ids::EdgeId edgeId) {
        return m_edges.get(edgeId);
    }
    const Core::Edge* getEdge(Core::Ids::EdgeId edgeId) const {
        return m_edges.get(edgeId);
    }

    void forEachEdgeId(const std::function<void(Core::Ids::EdgeId)>& f) const {
        for (const auto& e : m_edges.instances)
            f(e.id);
    }

  private:
    static bool contains(const std::vector<Core::Ids::EdgeId>& values, Core::Ids::EdgeId value);
    static void eraseValue(std::vector<Core::Ids::EdgeId>& values, Core::Ids::EdgeId value);
};
} // namespace ScopeCanvas::Widget
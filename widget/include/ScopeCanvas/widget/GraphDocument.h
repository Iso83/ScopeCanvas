#pragma once

#include <ScopeCanvas/core/Connector.h>
#include <ScopeCanvas/core/Edge.h>
#include <ScopeCanvas/core/Node.h>
#include <ScopeCanvas/core/ids/IdRegistry.h>
#include <ScopeCanvas/routing/IGraphView.h>
#include <algorithm>
#include <vector>

namespace ScopeCanvas::Core {
template <typename TGraph, typename TGraphId> struct GraphInstanceRegistry {
    std::vector<TGraph> instances{};
    Ids::GraphRegistry<TGraphId> ids{};

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
    GraphInstanceRegistry<Node, Ids::NodeId> m_nodes{};
    GraphInstanceRegistry<Connector, Ids::ConnectorId> m_connectors{};
    GraphInstanceRegistry<Edge, Ids::EdgeId> m_edges{};

  public:
    GraphDocument() = default;

    Ids::NodeId createNode(Ids::NodeTypeId typeId);
    void removeNode(Ids::NodeId nodeId);
    Node* getNode(Ids::NodeId nodeId) {
        return m_nodes.get(nodeId);
    }
    const Node* getNode(Ids::NodeId nodeId) const {
        return m_nodes.get(nodeId);
    }

    Connector* getConnector(Ids::ConnectorId connectorId) {
        return m_connectors.get(connectorId);
    }
    const Connector* getConnector(Ids::ConnectorId connectorId) const {
        return m_connectors.get(connectorId);
    }
    glm::vec2 connectorWorld(const Node& node, std::size_t index) const;

    Ids::EdgeId connect(Ids::ConnectorId a, Ids::ConnectorId b);
    void disconnect(Ids::EdgeId edgeId);
    Edge* getEdge(Ids::EdgeId edgeId) {
        return m_edges.get(edgeId);
    }
    const Edge* getEdge(Ids::EdgeId edgeId) const {
        return m_edges.get(edgeId);
    }

    void forEachEdgeId(const std::function<void(Core::Ids::EdgeId)>& f) const {
        for (const auto& e : m_edges.instances)
            f(e.id);
    }

  private:
    static bool contains(const std::vector<Ids::EdgeId>& values, Ids::EdgeId value);
    static void eraseValue(std::vector<Ids::EdgeId>& values, Ids::EdgeId value);
};
} // namespace ScopeCanvas::Core
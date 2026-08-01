#pragma once

#include <ScopeCanvas/engine/core/Connector.h>
#include <ScopeCanvas/engine/core/Edge.h>
#include <ScopeCanvas/engine/core/Node.h>
#include <ScopeCanvas/engine/core/ids/IdRegistry.h>
#include <ScopeCanvas/engine/routing/IGraphView.h>
#include <algorithm>
#include <vector>

namespace ScopeCanvas::Widget {
template <typename TGraph, typename TGraphId> struct GraphInstanceRegistry {
    std::vector<TGraph> instances{};
    Engine::Core::Ids::GraphRegistry<TGraphId> ids{};

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

class GraphDocument : public Engine::Routing::IGraphView {
private:
    GraphInstanceRegistry<Engine::Core::Node, Engine::Core::Ids::NodeId> m_nodes{};
    GraphInstanceRegistry<Engine::Core::Connector, Engine::Core::Ids::ConnectorId> m_connectors{};
    GraphInstanceRegistry<Engine::Core::Edge, Engine::Core::Ids::EdgeId> m_edges{};

public:
    GraphDocument() = default;

    Engine::Core::Ids::NodeId createNode(Engine::Core::Ids::NodeTypeId typeId);
    void removeNode(Engine::Core::Ids::NodeId nodeId);
    Engine::Core::Node* getNode(Engine::Core::Ids::NodeId nodeId) {
        return m_nodes.get(nodeId);
    }
    const Engine::Core::Node* getNode(Engine::Core::Ids::NodeId nodeId) const {
        return m_nodes.get(nodeId);
    }

    Engine::Core::Connector* getConnector(Engine::Core::Ids::ConnectorId connectorId) {
        return m_connectors.get(connectorId);
    }
    const Engine::Core::Connector* getConnector(Engine::Core::Ids::ConnectorId connectorId) const {
        return m_connectors.get(connectorId);
    }
    glm::vec2 connectorWorld(const Engine::Core::Node& node, std::size_t index) const;

    Engine::Core::Ids::EdgeId connect(Engine::Core::Ids::ConnectorId a, Engine::Core::Ids::ConnectorId b);
    void disconnect(Engine::Core::Ids::EdgeId edgeId);
    Engine::Core::Edge* getEdge(Engine::Core::Ids::EdgeId edgeId) {
        return m_edges.get(edgeId);
    }
    const Engine::Core::Edge* getEdge(Engine::Core::Ids::EdgeId edgeId) const {
        return m_edges.get(edgeId);
    }

    void forEachEdgeId(const std::function<void(Engine::Core::Ids::EdgeId)>& f) const {
        for (const auto& e : m_edges.instances)
            f(e.id);
    }

private:
    static bool contains(const std::vector<Engine::Core::Ids::EdgeId>& values, Engine::Core::Ids::EdgeId value);
    static void eraseValue(std::vector<Engine::Core::Ids::EdgeId>& values, Engine::Core::Ids::EdgeId value);
};
} // namespace ScopeCanvas::Widget

#pragma once

#include <cstdint>
#include <vector>

#include "ScopeCanvasEngineCore/Core/Connector.h"
#include "ScopeCanvasEngineCore/Core/Edge.h"
#include "ScopeCanvasEngineCore/Core/Node.h"
#include "ScopeCanvasEngineCore/Layout/LayoutEngine.h"
#include "ScopeCanvasEngineCore/Layout/LayoutGroup.h"

namespace ScopeCanvas::Engine::Core
{
class DiagramModel
{
public:
    DiagramModel() = default;

    CanvasNodeId createNode(NodeTypeId typeId);
    void removeNode(CanvasNodeId nodeId);

    CanvasEdgeId connect(CanvasConnectorId a, CanvasConnectorId b);
    void disconnect(CanvasEdgeId edgeId);

    void addNodeToGroup(CanvasNodeId node, LayoutGroupId group);
    void removeNodeFromGroup(CanvasNodeId node, LayoutGroupId group);

    void setLayoutEngine(LayoutEngine& engine);
    void layoutAll();
    void layoutNodes(const std::vector<CanvasNodeId>& nodes);

    Node* getNode(CanvasNodeId nodeId);
    const Node* getNode(CanvasNodeId nodeId) const;
    Connector* getConnector(CanvasConnectorId connectorId);
    const Connector* getConnector(CanvasConnectorId connectorId) const;
    Edge* getEdge(CanvasEdgeId edgeId);
    const Edge* getEdge(CanvasEdgeId edgeId) const;
    LayoutGroup* getLayoutGroup(LayoutGroupId groupId);
    const LayoutGroup* getLayoutGroup(LayoutGroupId groupId) const;

private:
    static bool contains(const std::vector<CanvasNodeId>& values, CanvasNodeId value);
    static bool contains(const std::vector<CanvasConnectorId>& values, CanvasConnectorId value);
    static bool contains(const std::vector<CanvasEdgeId>& values, CanvasEdgeId value);
    static bool contains(const std::vector<LayoutGroupId>& values, LayoutGroupId value);
    static void eraseValue(std::vector<CanvasNodeId>& values, CanvasNodeId value);
    static void eraseValue(std::vector<CanvasConnectorId>& values, CanvasConnectorId value);
    static void eraseValue(std::vector<CanvasEdgeId>& values, CanvasEdgeId value);
    static void eraseValue(std::vector<LayoutGroupId>& values, LayoutGroupId value);

    std::uint32_t m_nextNodeId{1};
    std::uint32_t m_nextConnectorId{1};
    std::uint32_t m_nextEdgeId{1};

    std::vector<Node> m_nodes{};
    std::vector<Connector> m_connectors{};
    std::vector<Edge> m_edges{};
    std::vector<LayoutGroup> m_layoutGroups{};
    LayoutEngine* m_layoutEngine{};
};
} // namespace ScopeCanvas::Engine::Core

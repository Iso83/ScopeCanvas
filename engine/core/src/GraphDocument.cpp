#include <ScopeCanvas/core/GraphDocument.h>
#include <algorithm>

namespace ScopeCanvas::Core {
CanvasNodeId GraphDocument::createNode(NodeTypeId typeId) {
    const CanvasNodeId newNodeId{m_nextNodeId++};
    const CanvasConnectorId firstConnectorId{m_nextConnectorId++};
    const CanvasConnectorId secondConnectorId{m_nextConnectorId++};

    Node node{};
    node.id = newNodeId;
    node.typeId = typeId;
    node.connectors.push_back(firstConnectorId);
    node.connectors.push_back(secondConnectorId);
    m_nodes.push_back(node);

    Connector firstConnector{};
    firstConnector.id = firstConnectorId;
    firstConnector.nodeId = newNodeId;
    m_connectors.push_back(firstConnector);

    Connector secondConnector{};
    secondConnector.id = secondConnectorId;
    secondConnector.nodeId = newNodeId;
    m_connectors.push_back(secondConnector);

    return newNodeId;
}

void GraphDocument::removeNode(CanvasNodeId nodeId) {
    Node* node = getNode(nodeId);
    if (node == nullptr) {
        return;
    }

    const std::vector<CanvasConnectorId> connectors = node->connectors;
    for (CanvasConnectorId connectorId : connectors) {
        Connector* connector = getConnector(connectorId);
        if (connector == nullptr) {
            continue;
        }

        const std::vector<CanvasEdgeId> edgeIds = connector->edges;
        for (CanvasEdgeId edgeId : edgeIds) {
            disconnect(edgeId);
        }

        m_connectors.erase(std::remove_if(m_connectors.begin(), m_connectors.end(),
                                          [connectorId](const Connector& value) { return value.id == connectorId; }),
                           m_connectors.end());
    }

    for (LayoutGroupId groupId : node->groups) {
        if (LayoutGroup* group = getLayoutGroup(groupId); group != nullptr) {
            eraseValue(group->nodes, nodeId);
        }
    }

    m_nodes.erase(
        std::remove_if(m_nodes.begin(), m_nodes.end(), [nodeId](const Node& value) { return value.id == nodeId; }),
        m_nodes.end());
}

CanvasEdgeId GraphDocument::connect(CanvasConnectorId a, CanvasConnectorId b) {
    Connector* fromConnector = getConnector(a);
    Connector* toConnector = getConnector(b);
    if (fromConnector == nullptr || toConnector == nullptr) {
        return CanvasEdgeId{};
    }

    const CanvasEdgeId edgeId{m_nextEdgeId++};
    m_edges.push_back(Edge{edgeId, a, b});

    if (!contains(fromConnector->edges, edgeId)) {
        fromConnector->edges.push_back(edgeId);
    }

    if (!contains(toConnector->edges, edgeId)) {
        toConnector->edges.push_back(edgeId);
    }

    return edgeId;
}

void GraphDocument::disconnect(CanvasEdgeId edgeId) {
    Edge* edge = getEdge(edgeId);
    if (edge == nullptr) {
        return;
    }

    const CanvasConnectorId fromId = edge->fromConnector;
    const CanvasConnectorId toId = edge->toConnector;

    if (Connector* fromConnector = getConnector(fromId); fromConnector != nullptr) {
        eraseValue(fromConnector->edges, edgeId);
    }

    if (Connector* toConnector = getConnector(toId); toConnector != nullptr) {
        eraseValue(toConnector->edges, edgeId);
    }

    m_edges.erase(
        std::remove_if(m_edges.begin(), m_edges.end(), [edgeId](const Edge& value) { return value.id == edgeId; }),
        m_edges.end());
}

void GraphDocument::addNodeToGroup(CanvasNodeId nodeId, LayoutGroupId groupId) {
    Node* node = getNode(nodeId);
    if (node == nullptr) {
        return;
    }

    LayoutGroup* group = getLayoutGroup(groupId);
    if (group == nullptr) {
        m_layoutGroups.push_back(LayoutGroup{});
        group = &m_layoutGroups.back();
        group->id = groupId;
    }

    if (!contains(node->groups, groupId)) {
        node->groups.push_back(groupId);
    }

    if (!contains(group->nodes, nodeId)) {
        group->nodes.push_back(nodeId);
    }
}

void GraphDocument::removeNodeFromGroup(CanvasNodeId nodeId, LayoutGroupId groupId) {
    Node* node = getNode(nodeId);
    LayoutGroup* group = getLayoutGroup(groupId);
    if (node == nullptr || group == nullptr) {
        return;
    }

    eraseValue(node->groups, groupId);
    eraseValue(group->nodes, nodeId);
}

void GraphDocument::setLayoutEngine(LayoutEngine& engine) {
    m_layoutEngine = &engine;
}

void GraphDocument::layoutAll() {
    if (m_layoutEngine != nullptr) {
        m_layoutEngine->layout(*this);
    }
}

void GraphDocument::layoutNodes(const std::vector<CanvasNodeId>& nodes) {
    if (m_layoutEngine != nullptr) {
        m_layoutEngine->layoutNodes(*this, nodes);
    }
}

Node* GraphDocument::getNode(CanvasNodeId nodeId) {
    const auto it =
        std::find_if(m_nodes.begin(), m_nodes.end(), [nodeId](const Node& node) { return node.id == nodeId; });
    return it == m_nodes.end() ? nullptr : &(*it);
}

const Node* GraphDocument::getNode(CanvasNodeId nodeId) const {
    const auto it =
        std::find_if(m_nodes.begin(), m_nodes.end(), [nodeId](const Node& node) { return node.id == nodeId; });
    return it == m_nodes.end() ? nullptr : &(*it);
}

Connector* GraphDocument::getConnector(CanvasConnectorId connectorId) {
    const auto it = std::find_if(m_connectors.begin(), m_connectors.end(),
                                 [connectorId](const Connector& connector) { return connector.id == connectorId; });
    return it == m_connectors.end() ? nullptr : &(*it);
}

const Connector* GraphDocument::getConnector(CanvasConnectorId connectorId) const {
    const auto it = std::find_if(m_connectors.begin(), m_connectors.end(),
                                 [connectorId](const Connector& connector) { return connector.id == connectorId; });
    return it == m_connectors.end() ? nullptr : &(*it);
}

Edge* GraphDocument::getEdge(CanvasEdgeId edgeId) {
    const auto it =
        std::find_if(m_edges.begin(), m_edges.end(), [edgeId](const Edge& edge) { return edge.id == edgeId; });
    return it == m_edges.end() ? nullptr : &(*it);
}

const Edge* GraphDocument::getEdge(CanvasEdgeId edgeId) const {
    const auto it =
        std::find_if(m_edges.begin(), m_edges.end(), [edgeId](const Edge& edge) { return edge.id == edgeId; });
    return it == m_edges.end() ? nullptr : &(*it);
}

LayoutGroup* GraphDocument::getLayoutGroup(LayoutGroupId groupId) {
    const auto it = std::find_if(m_layoutGroups.begin(), m_layoutGroups.end(),
                                 [groupId](const LayoutGroup& group) { return group.id == groupId; });
    return it == m_layoutGroups.end() ? nullptr : &(*it);
}

const LayoutGroup* GraphDocument::getLayoutGroup(LayoutGroupId groupId) const {
    const auto it = std::find_if(m_layoutGroups.begin(), m_layoutGroups.end(),
                                 [groupId](const LayoutGroup& group) { return group.id == groupId; });
    return it == m_layoutGroups.end() ? nullptr : &(*it);
}

bool GraphDocument::contains(const std::vector<CanvasNodeId>& values, CanvasNodeId value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

bool GraphDocument::contains(const std::vector<CanvasConnectorId>& values, CanvasConnectorId value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

bool GraphDocument::contains(const std::vector<CanvasEdgeId>& values, CanvasEdgeId value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

bool GraphDocument::contains(const std::vector<LayoutGroupId>& values, LayoutGroupId value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

void GraphDocument::eraseValue(std::vector<CanvasNodeId>& values, CanvasNodeId value) {
    values.erase(std::remove(values.begin(), values.end(), value), values.end());
}

void GraphDocument::eraseValue(std::vector<CanvasConnectorId>& values, CanvasConnectorId value) {
    values.erase(std::remove(values.begin(), values.end(), value), values.end());
}

void GraphDocument::eraseValue(std::vector<CanvasEdgeId>& values, CanvasEdgeId value) {
    values.erase(std::remove(values.begin(), values.end(), value), values.end());
}

void GraphDocument::eraseValue(std::vector<LayoutGroupId>& values, LayoutGroupId value) {
    values.erase(std::remove(values.begin(), values.end(), value), values.end());
}
} // namespace ScopeCanvas::Core

#include "Engine/DiagramModel.h"
#include "Engine/NodeTypes.h"

#include <algorithm>
#include <iostream>

namespace {
glm::vec2 sidePosition(const Node& node, ConnectorSide side, float offset) {
    const float clampedOffset = std::clamp(offset, 0.0f, 1.0f);
    switch (side) {
    case ConnectorSide::Top:
        return {node.position.x + node.size.x * clampedOffset, node.position.y};
    case ConnectorSide::Right:
        return {node.position.x + node.size.x, node.position.y + node.size.y * clampedOffset};
    case ConnectorSide::Bottom:
        return {node.position.x + node.size.x * clampedOffset, node.position.y + node.size.y};
    case ConnectorSide::Left:
        return {node.position.x, node.position.y + node.size.y * clampedOffset};
    }

    return node.position;
}


EdgeRoute buildOrthogonalRoute(const glm::vec2& start, const glm::vec2& end) {
    const float midX = (start.x + end.x) * 0.5f;

    EdgeRoute route;
    route.points.reserve(4);
    route.points.push_back(start);
    route.points.push_back({midX, start.y});
    route.points.push_back({midX, end.y});
    route.points.push_back(end);
    return route;
}
}

glm::vec2 connectorWorldPosition(const Node& node, const Connector& connector) {
    return sidePosition(node, connector.side, connector.offset);
}

std::vector<Connector> createDefaultConnectors(uint32_t nodeId, uint32_t& nextConnectorId) {
    return {
        Connector{nextConnectorId++, nodeId, ConnectorSide::Left, 0.333f, ConnectorDirection::Input, 1},
        Connector{nextConnectorId++, nodeId, ConnectorSide::Left, 0.666f, ConnectorDirection::Input, 1},
        Connector{nextConnectorId++, nodeId, ConnectorSide::Right, 0.5f, ConnectorDirection::Output, 1},
    };
}

DiagramModel::DiagramModel() : m_nodeTypeRegistry(std::make_unique<NodeTypeRegistry>()) {
    m_nodeTypeRegistry->registerBuiltInTypes();
}

DiagramModel::~DiagramModel() = default;

Node* DiagramModel::createNode(const glm::vec2& position, const glm::vec2& size) {
    return createNodeOfType("Add", position, size);
}

Node* DiagramModel::createNodeOfType(const std::string& nodeTypeId,
                                     const glm::vec2& position,
                                     const glm::vec2& size) {
    const NodeType* nodeType = m_nodeTypeRegistry->getType(nodeTypeId);
    if (nodeType == nullptr) {
        return nullptr;
    }

    std::vector<ConnectorTemplate> templates;
    templates.reserve(nodeType->inputs.size() + nodeType->outputs.size());
    templates.insert(templates.end(), nodeType->inputs.begin(), nodeType->inputs.end());
    templates.insert(templates.end(), nodeType->outputs.begin(), nodeType->outputs.end());
    return createNodeWithConnectors(position, size, templates, nodeType->typeId, nodeType->name);
}

Node* DiagramModel::createNodeWithConnectors(const glm::vec2& position,
                                             const glm::vec2& size,
                                             const std::vector<ConnectorTemplate>& connectors,
                                             const std::string& nodeTypeId,
                                             const std::string& title) {
    Node node{};
    node.id = m_nextNodeId++;
    node.nodeTypeId = nodeTypeId;
    node.title = title;
    node.position = position;
    node.size = size;
    node.selected = false;
    node.connectors.reserve(connectors.size());

    for (const ConnectorTemplate& connectorTemplate : connectors) {
        node.connectors.push_back(
            Connector{m_nextConnectorId++,
                      node.id,
                      connectorTemplate.side,
                      connectorTemplate.offset,
                      connectorTemplate.direction,
                      connectorTemplate.maxConnections});
    }

    m_nodes.push_back(node);
    return &m_nodes.back();
}

Node* DiagramModel::addNode(const Node& node) {
    if (findNode(node.id) != nullptr) {
        return nullptr;
    }

    m_nodes.push_back(node);
    syncIdCounters();
    return &m_nodes.back();
}

Node* DiagramModel::duplicateNode(uint32_t nodeId, const glm::vec2& offset) {
    const Node* sourceNode = findNode(nodeId);
    if (sourceNode == nullptr) {
        return nullptr;
    }

    return createNodeOfType(sourceNode->nodeTypeId, sourceNode->position + offset, sourceNode->size);
}

bool DiagramModel::removeNode(uint32_t nodeId) {
    const auto oldSize = m_nodes.size();
    m_nodes.erase(std::remove_if(m_nodes.begin(), m_nodes.end(),
                                 [nodeId](const Node& node) { return node.id == nodeId; }),
                  m_nodes.end());

    const bool removedNode = m_nodes.size() != oldSize;
    if (!removedNode) {
        return false;
    }

    removeEdgesForNode(nodeId);
    return true;
}

size_t DiagramModel::removeSelectedNodes() {
    std::vector<uint32_t> selectedNodeIds;
    selectedNodeIds.reserve(m_nodes.size());

    for (const Node& node : m_nodes) {
        if (node.selected) {
            selectedNodeIds.push_back(node.id);
        }
    }

    for (uint32_t nodeId : selectedNodeIds) {
        removeNode(nodeId);
    }

    return selectedNodeIds.size();
}


void DiagramModel::clear() {
    m_nodes.clear();
    m_edges.clear();
    m_groups.clear();
    m_nextNodeId = 1;
    m_nextConnectorId = 1;
    m_nextEdgeId = 1;
    m_nextGroupId = 1;
}

void DiagramModel::clearNodeSelection() {
    for (Node& node : m_nodes) {
        node.selected = false;
    }
}

bool DiagramModel::isValidConnection(uint32_t fromNode,
                                     uint32_t fromConnector,
                                     uint32_t toNode,
                                     uint32_t toConnector) const {
    if (fromNode == toNode) {
        return false;
    }

    if (findNode(fromNode) == nullptr || findNode(toNode) == nullptr) {
        return false;
    }

    const Connector* fromConnectorPtr = findConnector(fromNode, fromConnector);
    const Connector* toConnectorPtr = findConnector(toNode, toConnector);
    if (fromConnectorPtr == nullptr || toConnectorPtr == nullptr) {
        return false;
    }

    if (fromConnectorPtr->direction != ConnectorDirection::Output ||
        toConnectorPtr->direction != ConnectorDirection::Input) {
        return false;
    }

    const auto connectionCountFor = [this](uint32_t nodeId, uint32_t connectorId) {
        int currentConnections = 0;
        for (const Edge& edge : m_edges) {
            if ((edge.fromNode == nodeId && edge.fromConnector == connectorId) ||
                (edge.toNode == nodeId && edge.toConnector == connectorId)) {
                ++currentConnections;
            }
        }

        return currentConnections;
    };

    if (connectionCountFor(fromNode, fromConnector) >= fromConnectorPtr->maxConnections) {
        return false;
    }

    if (connectionCountFor(toNode, toConnector) >= toConnectorPtr->maxConnections) {
        return false;
    }

    return true;
}

bool DiagramModel::addEdge(const Edge& edge) {
    if (!isValidConnection(edge.fromNode, edge.fromConnector, edge.toNode, edge.toConnector)) {
        return false;
    }

    for (const Edge& existingEdge : m_edges) {
        if (existingEdge.id == edge.id) {
            return false;
        }
    }

    const Node* fromNode = findNode(edge.fromNode);
    const Node* toNode = findNode(edge.toNode);
    const Connector* fromConnector = findConnector(edge.fromNode, edge.fromConnector);
    const Connector* toConnector = findConnector(edge.toNode, edge.toConnector);
    if (fromNode == nullptr || toNode == nullptr || fromConnector == nullptr || toConnector == nullptr) {
        return false;
    }

    Edge routedEdge = edge;
    const glm::vec2 start = connectorWorldPosition(*fromNode, *fromConnector);
    const glm::vec2 end = connectorWorldPosition(*toNode, *toConnector);
    routedEdge.route = buildOrthogonalRoute(start, end);

    m_edges.push_back(routedEdge);
    if (routedEdge.id >= m_nextEdgeId) {
        m_nextEdgeId = routedEdge.id + 1;
    }

    std::cout << "Edge created: " << routedEdge.id << "\n";
    return true;
}

bool DiagramModel::createEdge(uint32_t fromNode,
                              uint32_t fromConnector,
                              uint32_t toNode,
                              uint32_t toConnector) {
    return addEdge(Edge{m_nextEdgeId++, fromNode, fromConnector, toNode, toConnector, false});
}

Group* DiagramModel::createGroup() {
    m_groups.push_back(Group{m_nextGroupId++});
    return &m_groups.back();
}

bool DiagramModel::addNodeToGroup(uint32_t nodeId, uint32_t groupId) {
    Node* node = findNode(nodeId);
    if (node == nullptr) {
        return false;
    }

    auto groupIt = std::find_if(m_groups.begin(), m_groups.end(),
                                [groupId](const Group& group) { return group.id == groupId; });
    if (groupIt == m_groups.end()) {
        return false;
    }

    if (node->groupId != 0 && node->groupId != groupId) {
        auto previousGroupIt = std::find_if(m_groups.begin(), m_groups.end(),
                                            [node](const Group& group) { return group.id == node->groupId; });
        if (previousGroupIt != m_groups.end()) {
            auto& previousChildren = previousGroupIt->children;
            previousChildren.erase(std::remove(previousChildren.begin(), previousChildren.end(), nodeId),
                                   previousChildren.end());
        }
    }

    node->groupId = groupId;

    if (std::find(groupIt->children.begin(), groupIt->children.end(), nodeId) == groupIt->children.end()) {
        groupIt->children.push_back(nodeId);
    }

    return true;
}

void DiagramModel::collapseGroup(uint32_t groupId) {
    auto groupIt = std::find_if(m_groups.begin(), m_groups.end(),
                                [groupId](const Group& group) { return group.id == groupId; });
    if (groupIt != m_groups.end()) {
        groupIt->collapsed = true;
    }
}

void DiagramModel::expandGroup(uint32_t groupId) {
    auto groupIt = std::find_if(m_groups.begin(), m_groups.end(),
                                [groupId](const Group& group) { return group.id == groupId; });
    if (groupIt != m_groups.end()) {
        groupIt->collapsed = false;
    }
}

bool DiagramModel::removeEdge(uint32_t edgeId) {
    const auto oldSize = m_edges.size();
    m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(),
                                 [edgeId](const Edge& edge) { return edge.id == edgeId; }),
                  m_edges.end());
    const bool removed = m_edges.size() != oldSize;
    if (removed) {
        std::cout << "Edge removed: " << edgeId << "\n";
    }
    return removed;
}

void DiagramModel::syncIdCounters() {
    for (const Node& node : m_nodes) {
        if (node.id >= m_nextNodeId) {
            m_nextNodeId = node.id + 1;
        }

        for (const Connector& connector : node.connectors) {
            if (connector.id >= m_nextConnectorId) {
                m_nextConnectorId = connector.id + 1;
            }
        }
    }

    for (const Edge& edge : m_edges) {
        if (edge.id >= m_nextEdgeId) {
            m_nextEdgeId = edge.id + 1;
        }
    }

    for (const Group& group : m_groups) {
        if (group.id >= m_nextGroupId) {
            m_nextGroupId = group.id + 1;
        }
    }
}

Node* DiagramModel::findNode(uint32_t nodeId) {
    for (Node& node : m_nodes) {
        if (node.id == nodeId) {
            return &node;
        }
    }

    return nullptr;
}

const Node* DiagramModel::findNode(uint32_t nodeId) const {
    for (const Node& node : m_nodes) {
        if (node.id == nodeId) {
            return &node;
        }
    }

    return nullptr;
}

Connector* DiagramModel::findConnector(uint32_t nodeId, uint32_t connectorId) {
    Node* node = findNode(nodeId);
    if (node == nullptr) {
        return nullptr;
    }

    for (Connector& connector : node->connectors) {
        if (connector.id == connectorId) {
            return &connector;
        }
    }

    return nullptr;
}

const Connector* DiagramModel::findConnector(uint32_t nodeId, uint32_t connectorId) const {
    const Node* node = findNode(nodeId);
    if (node == nullptr) {
        return nullptr;
    }

    for (const Connector& connector : node->connectors) {
        if (connector.id == connectorId) {
            return &connector;
        }
    }

    return nullptr;
}

Edge* DiagramModel::findEdge(uint32_t edgeId) {
    for (Edge& edge : m_edges) {
        if (edge.id == edgeId) {
            return &edge;
        }
    }

    return nullptr;
}

const Edge* DiagramModel::findEdge(uint32_t edgeId) const {
    for (const Edge& edge : m_edges) {
        if (edge.id == edgeId) {
            return &edge;
        }
    }

    return nullptr;
}

const NodeTypeRegistry& DiagramModel::nodeTypeRegistry() const {
    return *m_nodeTypeRegistry;
}

bool DiagramModel::removeEdgesForNode(uint32_t nodeId) {
    const auto oldSize = m_edges.size();
    m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(),
                                 [nodeId](const Edge& edge) {
                                     return edge.fromNode == nodeId || edge.toNode == nodeId;
                                 }),
                  m_edges.end());
    return m_edges.size() != oldSize;
}

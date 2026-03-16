#include "Engine/DiagramModel.h"
#include "Engine/NodeTypes.h"

#include <algorithm>
#include <iostream>

namespace {
class DefaultConnectionRule final : public IConnectionRule {
public:
    bool canConnect(const DiagramModel &model,
        const Connector &from,
        const Connector &to,
        std::string *reason) const override {
        if (from.nodeId == to.nodeId && from.id == to.id) {
            if (reason != nullptr) {
                *reason = "cannot connect a connector to itself";
            }
            return false;
        }

        if (from.direction != ConnectorDirection::Output || to.direction != ConnectorDirection::Input) {
            if (reason != nullptr) {
                *reason = "connections must be output -> input";
            }
            return false;
        }

        auto connectionCountFor = [&model](CanvasNodeId nodeId, CanvasConnectorId connectorId) {
            int currentConnections = 0;
            for (const Edge &edge : model.edges()) {
                if ((edge.fromNode == nodeId && edge.fromConnector == connectorId) ||
                    (edge.toNode == nodeId && edge.toConnector == connectorId)) {
                    ++currentConnections;
                }
            }
            return currentConnections;
        };

        if (connectionCountFor(from.nodeId, from.id) >= from.maxConnections) {
            if (reason != nullptr) {
                *reason = "source connector reached max connections";
            }
            return false;
        }
        if (connectionCountFor(to.nodeId, to.id) >= to.maxConnections) {
            if (reason != nullptr) {
                *reason = "target connector reached max connections";
            }
            return false;
        }

        for (const Edge &edge : model.edges()) {
            if (edge.fromNode == from.nodeId && edge.fromConnector == from.id &&
                edge.toNode == to.nodeId && edge.toConnector == to.id) {
                if (reason != nullptr) {
                    *reason = "duplicate edge";
                }
                return false;
            }
        }

        return true;
    }
};

    glm::vec2 sidePosition(const Node &node, ConnectorSide side, float offset) {
        const float clampedOffset = std::clamp(offset, 0.0f, 1.0f);
        switch (side) {
        case ConnectorSide::Top:
            return { node.position.x + node.size.x * clampedOffset, node.position.y };
        case ConnectorSide::Right:
            return { node.position.x + node.size.x, node.position.y + node.size.y * clampedOffset };
        case ConnectorSide::Bottom:
            return { node.position.x + node.size.x * clampedOffset, node.position.y + node.size.y };
        case ConnectorSide::Left:
            return { node.position.x, node.position.y + node.size.y * clampedOffset };
        }

        return node.position;
    }


    EdgeRoute buildOrthogonalRoute(const glm::vec2 &start, const glm::vec2 &end) {
        const float midX = (start.x + end.x) * 0.5f;

        EdgeRoute route;
        route.points.reserve(4);
        route.points.push_back(start);
        route.points.push_back({ midX, start.y });
        route.points.push_back({ midX, end.y });
        route.points.push_back(end);
        return route;
    }
}

glm::vec2 connectorWorldPosition(const Node &node, const Connector &connector) {
    return sidePosition(node, connector.side, connector.offset);
}

std::vector<Connector> createDefaultConnectors(CanvasNodeId nodeId, uint32_t &nextConnectorId) {
    return {
        Connector{nextConnectorId++, nodeId, ConnectorSide::Left, 0.333f, ConnectorDirection::Input, 1},
        Connector{nextConnectorId++, nodeId, ConnectorSide::Left, 0.666f, ConnectorDirection::Input, 1},
        Connector{nextConnectorId++, nodeId, ConnectorSide::Right, 0.5f, ConnectorDirection::Output, 1},
    };
}

DiagramModel::DiagramModel() : m_nodeTypeRegistry(std::make_unique<NodeTypeRegistry>()) {
    m_nodeTypeRegistry->registerBuiltInTypes();
    m_connectionRule = std::make_shared<DefaultConnectionRule>();
}

DiagramModel::~DiagramModel() = default;

Node *DiagramModel::createNode(const glm::vec2 &position, const glm::vec2 &size) {
    return createNodeOfType("Add", position, size);
}

Node *DiagramModel::createNodeOfType(const std::string &nodeTypeId,
    const glm::vec2 &position,
    const glm::vec2 &size) {
    const NodeType *nodeType = m_nodeTypeRegistry->getType(nodeTypeId);
    if (nodeType == nullptr) {
        return nullptr;
    }

    std::vector<ConnectorTemplate> templates;
    templates.reserve(nodeType->inputs.size() + nodeType->outputs.size());
    templates.insert(templates.end(), nodeType->inputs.begin(), nodeType->inputs.end());
    templates.insert(templates.end(), nodeType->outputs.begin(), nodeType->outputs.end());
    Node *node = createNodeWithConnectors(position, size, templates, nodeType->typeId, nodeType->name);
    if (node != nullptr) {
        node->allowChildren = nodeType->allowChildren;
    }
    return node;
}

Node *DiagramModel::createNodeWithConnectors(const glm::vec2 &position,
    const glm::vec2 &size,
    const std::vector<ConnectorTemplate> &connectors,
    const std::string &nodeTypeId,
    const std::string &title) {
    Node node{};
    node.id = m_nextNodeId++;
    node.nodeTypeId = nodeTypeId;
    node.title = title;
    node.position = position;
    node.size = size;
    node.selected = false;
    node.allowChildren = false;
    node.connectors.reserve(connectors.size());

    int inputSlot = 0;
    int outputSlot = 0;
    for (const ConnectorTemplate &connectorTemplate : connectors) {
        Connector connector{ m_nextConnectorId++, node.id, connectorTemplate.side,
            connectorTemplate.offset, connectorTemplate.direction, connectorTemplate.maxConnections };
        connector.semanticSlot = connector.direction == ConnectorDirection::Input ? inputSlot++ : outputSlot++;
        node.connectors.push_back(connector);
    }

    m_nodes.push_back(node);
    return &m_nodes.back();
}

Node *DiagramModel::addNode(const Node &node) {
    if (findNode(node.id) != nullptr) {
        return nullptr;
    }

    m_nodes.push_back(node);
    syncIdCounters();
    return &m_nodes.back();
}

Node *DiagramModel::duplicateNode(CanvasNodeId nodeId, const glm::vec2 &offset) {
    const Node *sourceNode = findNode(nodeId);
    if (sourceNode == nullptr) {
        return nullptr;
    }

    return createNodeOfType(sourceNode->nodeTypeId, sourceNode->position + offset, sourceNode->size);
}

bool DiagramModel::removeNode(CanvasNodeId nodeId) {
    Node *nodeToDelete = findNode(nodeId);
    if (nodeToDelete != nullptr) {
        for (CanvasNodeId childId : nodeToDelete->children) {
            Node *child = findNode(childId);
            if (child != nullptr) {
                child->parentId = CanvasNodeId{};
            }
        }

        if (nodeToDelete->parentId) {
            Node *parent = findNode(nodeToDelete->parentId);
            if (parent != nullptr) {
                std::erase(parent->children, nodeId);
            }
        }
    }

    const auto oldSize = m_nodes.size();
    m_nodes.erase(std::remove_if(m_nodes.begin(), m_nodes.end(),
        [nodeId](const Node &node) { return node.id == nodeId; }),
        m_nodes.end());

    const bool removedNode = m_nodes.size() != oldSize;
    if (!removedNode) {
        return false;
    }

    removeEdgesForNode(nodeId);
    return true;
}

size_t DiagramModel::removeSelectedNodes() {
    std::vector<CanvasNodeId> selectedNodeIds;
    selectedNodeIds.reserve(m_nodes.size());

    for (const Node &node : m_nodes) {
        if (node.selected) {
            selectedNodeIds.push_back(node.id);
        }
    }

    for (CanvasNodeId nodeId : selectedNodeIds) {
        removeNode(nodeId);
    }

    return selectedNodeIds.size();
}


void DiagramModel::clear() {
    m_nodes.clear();
    m_edges.clear();
    m_nextNodeId = 1;
    m_nextConnectorId = 1;
    m_nextEdgeId = 1;
}

void DiagramModel::clearNodeSelection() {
    for (Node &node : m_nodes) {
        node.selected = false;
    }
}

bool DiagramModel::isValidConnection(CanvasNodeId fromNode,
    CanvasConnectorId fromConnector,
    CanvasNodeId toNode,
    CanvasConnectorId toConnector,
    std::string *reason) const {
    if (fromNode == toNode) {
        if (reason != nullptr) {
            *reason = "cannot connect node to itself";
        }
        return false;
    }

    if (findNode(fromNode) == nullptr || findNode(toNode) == nullptr) {
        if (reason != nullptr) {
            *reason = "invalid node id";
        }
        return false;
    }

    const Connector *fromConnectorPtr = findConnector(fromNode, fromConnector);
    const Connector *toConnectorPtr = findConnector(toNode, toConnector);
    if (fromConnectorPtr == nullptr || toConnectorPtr == nullptr) {
        if (reason != nullptr) {
            *reason = "invalid connector id";
        }
        return false;
    }

    if (!m_connectionRule) {
        return true;
    }

    return m_connectionRule->canConnect(*this, *fromConnectorPtr, *toConnectorPtr, reason);
}

bool DiagramModel::addEdge(const Edge &edge) {
    if (!isValidConnection(edge.fromNode, edge.fromConnector, edge.toNode, edge.toConnector)) {
        return false;
    }

    for (const Edge &existingEdge : m_edges) {
        if (existingEdge.id == edge.id) {
            return false;
        }
    }

    const Node *fromNode = findNode(edge.fromNode);
    const Node *toNode = findNode(edge.toNode);
    const Connector *fromConnector = findConnector(edge.fromNode, edge.fromConnector);
    const Connector *toConnector = findConnector(edge.toNode, edge.toConnector);
    if (fromNode == nullptr || toNode == nullptr || fromConnector == nullptr || toConnector == nullptr) {
        return false;
    }

    Edge routedEdge = edge;
    const glm::vec2 start = connectorWorldPosition(*fromNode, *fromConnector);
    const glm::vec2 end = connectorWorldPosition(*toNode, *toConnector);
    routedEdge.route = buildOrthogonalRoute(start, end);

    m_edges.push_back(routedEdge);
    if (routedEdge.id.value >= m_nextEdgeId) {
        m_nextEdgeId = routedEdge.id.value + 1;
    }

    std::cout << "Edge created: " << routedEdge.id.value << "\n";
    return true;
}

bool DiagramModel::createEdge(CanvasNodeId fromNode,
    CanvasConnectorId fromConnector,
    CanvasNodeId toNode,
    CanvasConnectorId toConnector) {
    return addEdge(Edge{ m_nextEdgeId++, fromNode, fromConnector, toNode, toConnector, false });
}

void DiagramModel::recomputeRoutesForNode(CanvasNodeId nodeId) {
    for (Edge &edge : m_edges) {
        if (edge.fromNode != nodeId && edge.toNode != nodeId) {
            continue;
        }

        const Node *fromNode = findNode(edge.fromNode);
        const Node *toNode = findNode(edge.toNode);
        const Connector *fromConnector = findConnector(edge.fromNode, edge.fromConnector);
        const Connector *toConnector = findConnector(edge.toNode, edge.toConnector);
        if (fromNode == nullptr || toNode == nullptr || fromConnector == nullptr || toConnector == nullptr) {
            edge.route.points.clear();
            continue;
        }

        const glm::vec2 start = connectorWorldPosition(*fromNode, *fromConnector);
        const glm::vec2 end = connectorWorldPosition(*toNode, *toConnector);
        edge.route = buildOrthogonalRoute(start, end);
    }
}

bool DiagramModel::addChildNode(CanvasNodeId parentId, CanvasNodeId childId) {
    if (parentId == childId) {
        return false;
    }

    Node *parent = findNode(parentId);
    Node *child = findNode(childId);
    if (parent == nullptr || child == nullptr || !parent->allowChildren) {
        return false;
    }

    if (child->parentId) {
        detachFromParent(child->id);
    }

    child->parentId = parentId;
    if (std::find(parent->children.begin(), parent->children.end(), childId) == parent->children.end()) {
        parent->children.push_back(childId);
    }

    return true;
}

bool DiagramModel::detachFromParent(CanvasNodeId nodeId) {
    Node *node = findNode(nodeId);
    if (node == nullptr || !node->parentId) {
        return false;
    }

    Node *parent = findNode(node->parentId);
    if (parent != nullptr) {
        std::erase(parent->children, node->id);
    }

    node->parentId = CanvasNodeId{};
    return true;
}

void DiagramModel::setNodeCollapsed(CanvasNodeId nodeId, bool collapsed) {
    Node *node = findNode(nodeId);
    if (node != nullptr && node->allowChildren) {
        node->collapsed = collapsed;
    }
}

bool DiagramModel::isNodeHiddenByCollapsedAncestor(CanvasNodeId nodeId) const {
    const Node *node = findNode(nodeId);
    while (node != nullptr && node->parentId) {
        const Node *parent = findNode(node->parentId);
        if (parent == nullptr) {
            break;
        }
        if (parent->collapsed) {
            return true;
        }
        node = parent;
    }

    return false;
}

bool DiagramModel::removeEdge(CanvasEdgeId edgeId) {
    const auto oldSize = m_edges.size();
    m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(),
        [edgeId](const Edge &edge) { return edge.id == edgeId; }),
        m_edges.end());
    const bool removed = m_edges.size() != oldSize;
    if (removed) {
        std::cout << "Edge removed: " << edgeId.value << "\n";
    }
    return removed;
}

void DiagramModel::syncIdCounters() {
    for (const Node &node : m_nodes) {
        if (node.id.value >= m_nextNodeId) {
            m_nextNodeId = node.id.value + 1;
        }

        for (const Connector &connector : node.connectors) {
            if (connector.id.value >= m_nextConnectorId) {
                m_nextConnectorId = connector.id.value + 1;
            }
        }
    }

    for (const Edge &edge : m_edges) {
        if (edge.id.value >= m_nextEdgeId) {
            m_nextEdgeId = edge.id.value + 1;
        }
    }
}

Node *DiagramModel::findNode(CanvasNodeId nodeId) {
    for (Node &node : m_nodes) {
        if (node.id == nodeId) {
            return &node;
        }
    }

    return nullptr;
}

const Node *DiagramModel::findNode(CanvasNodeId nodeId) const {
    for (const Node &node : m_nodes) {
        if (node.id == nodeId) {
            return &node;
        }
    }

    return nullptr;
}

Connector *DiagramModel::findConnector(CanvasNodeId nodeId, CanvasConnectorId connectorId) {
    Node *node = findNode(nodeId);
    if (node == nullptr) {
        return nullptr;
    }

    for (Connector &connector : node->connectors) {
        if (connector.id == connectorId) {
            return &connector;
        }
    }

    return nullptr;
}

const Connector *DiagramModel::findConnector(CanvasNodeId nodeId, CanvasConnectorId connectorId) const {
    const Node *node = findNode(nodeId);
    if (node == nullptr) {
        return nullptr;
    }

    for (const Connector &connector : node->connectors) {
        if (connector.id == connectorId) {
            return &connector;
        }
    }

    return nullptr;
}

Edge *DiagramModel::findEdge(CanvasEdgeId edgeId) {
    for (Edge &edge : m_edges) {
        if (edge.id == edgeId) {
            return &edge;
        }
    }

    return nullptr;
}

const Edge *DiagramModel::findEdge(CanvasEdgeId edgeId) const {
    for (const Edge &edge : m_edges) {
        if (edge.id == edgeId) {
            return &edge;
        }
    }

    return nullptr;
}

void DiagramModel::setConnectionRule(std::shared_ptr<IConnectionRule> rule) {
    m_connectionRule = std::move(rule);
}

const NodeTypeRegistry &DiagramModel::nodeTypeRegistry() const {
    return *m_nodeTypeRegistry;
}

bool DiagramModel::removeEdgesForNode(CanvasNodeId nodeId) {
    const auto oldSize = m_edges.size();
    m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(),
        [nodeId](const Edge &edge) {
            return edge.fromNode == nodeId || edge.toNode == nodeId;
        }),
        m_edges.end());
    return m_edges.size() != oldSize;
}

#include "Engine/DiagramModel.h"

#include <algorithm>

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
}

glm::vec2 connectorWorldPosition(const Node& node, const Connector& connector) {
    return sidePosition(node, connector.side, connector.offset);
}

std::vector<Connector> createDefaultConnectors(uint32_t nodeId, uint32_t& nextConnectorId) {
    return {
        Connector{nextConnectorId++, nodeId, ConnectorSide::Top, 0.5f},
        Connector{nextConnectorId++, nodeId, ConnectorSide::Right, 0.5f},
        Connector{nextConnectorId++, nodeId, ConnectorSide::Bottom, 0.5f},
        Connector{nextConnectorId++, nodeId, ConnectorSide::Left, 0.5f},
    };
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

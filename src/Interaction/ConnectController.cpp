#include "Interaction/ConnectController.h"

#include "Interaction/EdgeInteractionController.h"

#include <iostream>

bool ConnectController::onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld, float zoom) {
    uint32_t startNodeId = 0;
    const Connector* connector = hitTestConnector(model, mouseWorld, zoom, startNodeId);
    if (connector == nullptr) {
        return false;
    }

    m_connecting = true;
    m_startNodeId = startNodeId;
    m_startConnectorId = connector->id;
    m_previewPosition = mouseWorld;
    return true;
}

void ConnectController::beginReconnect(uint32_t edgeId,
                                       bool startEndpoint,
                                       uint32_t fixedNodeId,
                                       uint32_t fixedConnectorId,
                                       const glm::vec2& mouseWorld) {
    m_connecting = true;
    m_reconnectActive = true;
    m_reconnectEdgeId = edgeId;
    m_reconnectStartEndpoint = startEndpoint;
    m_startNodeId = fixedNodeId;
    m_startConnectorId = fixedConnectorId;
    m_previewPosition = mouseWorld;
}

void ConnectController::onMouseMove(const glm::vec2& mouseWorld) {
    if (!m_connecting) {
        return;
    }

    m_previewPosition = mouseWorld;
}

bool ConnectController::onMouseUp(DiagramModel& model, const glm::vec2& mouseWorld, float zoom) {
    if (!m_connecting) {
        return false;
    }

    m_previewPosition = mouseWorld;

    uint32_t endNodeId = 0;
    const Connector* endConnector = hitTestConnector(model, mouseWorld, zoom, endNodeId);
    if (endConnector == nullptr) {
        std::cout << "Connection cancelled\n";
        reset();
        return true;
    }

    const bool sameEndpoint = endNodeId == m_startNodeId && endConnector->id == m_startConnectorId;

    bool duplicateEdge = false;
    if (!sameEndpoint) {
        for (const Edge& edge : model.edges()) {
            const bool sameDirection =
                edge.fromNode == m_startNodeId &&
                edge.fromConnector == m_startConnectorId &&
                edge.toNode == endNodeId &&
                edge.toConnector == endConnector->id;
            const bool reverseDirection =
                edge.fromNode == endNodeId &&
                edge.fromConnector == endConnector->id &&
                edge.toNode == m_startNodeId &&
                edge.toConnector == m_startConnectorId;
            if (sameDirection || reverseDirection) {
                duplicateEdge = true;
                break;
            }
        }
    }

    if (sameEndpoint || duplicateEdge) {
        std::cout << "Connection cancelled\n";
        reset();
        return true;
    }

    uint32_t edgeId = 1;
    if (m_reconnectActive) {
        edgeId = m_reconnectEdgeId;
    } else {
        for (const Edge& edge : model.edges()) {
            if (edge.id >= edgeId) {
                edgeId = edge.id + 1;
            }
        }
    }

    Edge newEdge{edgeId, m_startNodeId, m_startConnectorId, endNodeId, endConnector->id, false};
    if (m_reconnectActive && m_reconnectStartEndpoint) {
        newEdge.fromNode = endNodeId;
        newEdge.fromConnector = endConnector->id;
        newEdge.toNode = m_startNodeId;
        newEdge.toConnector = m_startConnectorId;
    }

    model.addEdge(newEdge);

    reset();
    return true;
}

const Connector* ConnectController::hitTestConnector(const DiagramModel& model,
                                                     const glm::vec2& mouseWorld,
                                                     float zoom,
                                                     uint32_t& nodeId) const {
    nodeId = 0;
    return EdgeInteractionController::hitTestConnector(model, mouseWorld, zoom, &nodeId);
}

void ConnectController::reset() {
    m_connecting = false;
    m_startNodeId = 0;
    m_startConnectorId = 0;
    m_reconnectEdgeId = 0;
    m_reconnectActive = false;
    m_reconnectStartEndpoint = false;
    m_previewPosition = glm::vec2(0.0f);
}

#include "Interaction/ConnectController.h"

#include <iostream>

#include <glm/geometric.hpp>

bool ConnectController::onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld) {
    Connector* connector = hitTestConnector(model, mouseWorld);
    if (connector == nullptr) {
        return false;
    }

    m_connecting = true;
    m_startConnector = connector;
    m_startNodeId = connector->nodeId;
    m_startConnectorId = connector->id;
    m_previewPosition = mouseWorld;
    return true;
}

void ConnectController::onMouseMove(const glm::vec2& mouseWorld) {
    if (!m_connecting) {
        return;
    }

    m_previewPosition = mouseWorld;
}

bool ConnectController::onMouseUp(DiagramModel& model, const glm::vec2& mouseWorld) {
    if (!m_connecting || m_startConnector == nullptr) {
        return false;
    }

    m_previewPosition = mouseWorld;
    Connector* endConnector = hitTestConnector(model, mouseWorld);

    if (endConnector == nullptr) {
        std::cout << "Connection cancelled\n";
    } else {
        const bool selfConnection =
            endConnector->nodeId == m_startNodeId && endConnector->id == m_startConnectorId;

        bool duplicateEdge = false;
        if (!selfConnection) {
            for (const Edge& edge : model.edges()) {
                const bool sameDirection =
                    edge.fromNode == m_startNodeId &&
                    edge.fromConnector == m_startConnectorId &&
                    edge.toNode == endConnector->nodeId &&
                    edge.toConnector == endConnector->id;
                const bool reverseDirection =
                    edge.fromNode == endConnector->nodeId &&
                    edge.fromConnector == endConnector->id &&
                    edge.toNode == m_startNodeId &&
                    edge.toConnector == m_startConnectorId;
                if (sameDirection || reverseDirection) {
                    duplicateEdge = true;
                    break;
                }
            }
        }

        if (selfConnection || duplicateEdge) {
            std::cout << "Connection cancelled\n";
        } else {
            uint32_t nextEdgeId = 1;
            for (const Edge& edge : model.edges()) {
                if (edge.id >= nextEdgeId) {
                    nextEdgeId = edge.id + 1;
                }
            }

            const Edge newEdge{
                nextEdgeId,
                m_startNodeId,
                m_startConnectorId,
                endConnector->nodeId,
                endConnector->id,
            };
            model.addEdge(newEdge);
        }
    }

    m_connecting = false;
    m_startConnector = nullptr;
    m_startNodeId = 0;
    m_startConnectorId = 0;
    m_previewPosition = glm::vec2(0.0f);
    return true;
}

Connector* ConnectController::hitTestConnector(DiagramModel& model, const glm::vec2& mouseWorld) const {
    for (auto nodeIt = model.nodes().rbegin(); nodeIt != model.nodes().rend(); ++nodeIt) {
        Node& node = *nodeIt;
        for (Connector& connector : node.connectors) {
            const glm::vec2 connectorPos = connectorWorldPosition(node, connector);
            if (glm::distance(mouseWorld, connectorPos) < kConnectorHitRadius) {
                return &connector;
            }
        }
    }

    return nullptr;
}

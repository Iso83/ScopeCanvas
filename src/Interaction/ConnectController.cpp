#include "Interaction/ConnectController.h"

#include <algorithm>

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
    if (endConnector != nullptr &&
        !(endConnector->nodeId == m_startNodeId && endConnector->id == m_startConnectorId)) {
        const auto maxEdgeIt = std::max_element(model.edges().begin(), model.edges().end(),
            [](const Edge& lhs, const Edge& rhs) {
                return lhs.id < rhs.id;
            });
        if (maxEdgeIt != model.edges().end()) {
            m_nextEdgeId = maxEdgeIt->id + 1;
        }

        model.edges().push_back(Edge{
            m_nextEdgeId++,
            m_startNodeId,
            m_startConnectorId,
            endConnector->nodeId,
            endConnector->id,
        });
    }

    m_connecting = false;
    m_startConnector = nullptr;
    m_startNodeId = 0;
    m_startConnectorId = 0;
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

#pragma once

#include "Engine/DiagramModel.h"

#include <glm/vec2.hpp>

class ConnectController {
public:
    struct ConnectionResult {
        bool handled = false;
        bool createEdge = false;
        Edge edge{};
    };

    bool onMouseDown(const DiagramModel& model, const glm::vec2& mouseWorld, float zoom, bool overrideMode);
    void onMouseMove(const glm::vec2& mouseWorld);
    ConnectionResult onMouseUp(const DiagramModel& model, const glm::vec2& mouseWorld, float zoom);

    void beginReconnect(uint32_t edgeId,
                        bool startEndpoint,
                        uint32_t fixedNodeId,
                        uint32_t fixedConnectorId,
                        const glm::vec2& mouseWorld);

    bool isConnecting() const { return m_connecting; }
    uint32_t startNodeId() const { return m_startNodeId; }
    uint32_t startConnectorId() const { return m_startConnectorId; }
    const glm::vec2& previewPosition() const { return m_previewPosition; }

private:
    const Connector* hitTestConnector(const DiagramModel& model,
                                      const glm::vec2& mouseWorld,
                                      float zoom,
                                      uint32_t& nodeId) const;

    static bool connectorHasEdge(const DiagramModel& model, uint32_t nodeId, uint32_t connectorId);
    static uint32_t allocateEdgeId(const DiagramModel& model);

    void reset();

    bool m_connecting = false;
    uint32_t m_startNodeId = 0;
    uint32_t m_startConnectorId = 0;
    uint32_t m_reconnectEdgeId = 0;
    bool m_reconnectActive = false;
    bool m_reconnectStartEndpoint = false;
    glm::vec2 m_previewPosition = glm::vec2(0.0f);
};

#pragma once

#include "Engine/DiagramModel.h"

#include <glm/vec2.hpp>

class ConnectController {
public:
    bool onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld);
    void onMouseMove(const glm::vec2& mouseWorld);
    bool onMouseUp(DiagramModel& model, const glm::vec2& mouseWorld);

    bool isConnecting() const { return m_connecting; }
    uint32_t startNodeId() const { return m_startNodeId; }
    uint32_t startConnectorId() const { return m_startConnectorId; }
    const glm::vec2& previewPosition() const { return m_previewPosition; }

private:
    static constexpr float kConnectorHitRadius = 6.0f;

    Connector* hitTestConnector(DiagramModel& model, const glm::vec2& mouseWorld) const;

    bool m_connecting = false;
    Connector* m_startConnector = nullptr;
    uint32_t m_startNodeId = 0;
    uint32_t m_startConnectorId = 0;
    glm::vec2 m_previewPosition = glm::vec2(0.0f);
    uint32_t m_nextEdgeId = 1;
};

#pragma once

#include <glm/vec2.hpp>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <vector>

namespace ScopeCanvas::Render::Scene {
struct NodeRenderData {
    Core::CanvasNodeId id{};
    glm::vec2 position{};
    glm::vec2 size{};
};

struct ConnectorAnchorRenderData {
    Core::CanvasConnectorId connectorId{};
    Core::CanvasNodeId nodeId{};
    glm::vec2 anchor{};
};

struct EdgeRenderData {
    Core::CanvasEdgeId edgeId{};
    std::vector<glm::vec2> points{};
};

struct RenderScene {
    std::vector<NodeRenderData> nodes{};
    std::vector<ConnectorAnchorRenderData> connectorAnchors{};
    std::vector<EdgeRenderData> edges{};
};
} // namespace ScopeCanvas::Render::Scene

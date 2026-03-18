#pragma once

#include <ScopeCanvas/core/Vec2.h>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <vector>

namespace ScopeCanvas::Render::Scene {
struct NodeRenderData {
    Core::CanvasNodeId id{};
    Core::Vec2 position{};
    Core::Vec2 size{};
};

struct ConnectorAnchorRenderData {
    Core::CanvasConnectorId connectorId{};
    Core::CanvasNodeId nodeId{};
    Core::Vec2 anchor{};
};

struct EdgeRenderData {
    Core::CanvasEdgeId edgeId{};
    std::vector<Core::Vec2> points{};
};

struct RenderScene {
    std::vector<NodeRenderData> nodes{};
    std::vector<ConnectorAnchorRenderData> connectorAnchors{};
    std::vector<EdgeRenderData> edges{};
};
} // namespace ScopeCanvas::Render::Scene

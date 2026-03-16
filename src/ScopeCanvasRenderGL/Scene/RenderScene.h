#pragma once

#include <vector>

#include "ScopeCanvasEngineCore/Core/Vec2.h"
#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"

namespace ScopeCanvas::Render::Scene
{
struct NodeRenderData
{
    Engine::Core::CanvasNodeId id{};
    Engine::Core::Vec2 position{};
    Engine::Core::Vec2 size{};
};

struct ConnectorAnchorRenderData
{
    Engine::Core::CanvasConnectorId connectorId{};
    Engine::Core::CanvasNodeId nodeId{};
    Engine::Core::Vec2 anchor{};
};

struct EdgeRenderData
{
    Engine::Core::CanvasEdgeId edgeId{};
    std::vector<Engine::Core::Vec2> points{};
};

struct RenderScene
{
    std::vector<NodeRenderData> nodes{};
    std::vector<ConnectorAnchorRenderData> connectorAnchors{};
    std::vector<EdgeRenderData> edges{};
};
} // namespace ScopeCanvas::Render::Scene

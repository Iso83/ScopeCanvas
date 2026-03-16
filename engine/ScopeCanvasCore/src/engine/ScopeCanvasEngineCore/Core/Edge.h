#pragma once

#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"

namespace ScopeCanvas::Engine::Core
{
struct Edge
{
    CanvasEdgeId id{};
    CanvasConnectorId fromConnector{};
    CanvasConnectorId toConnector{};
};
} // namespace ScopeCanvas::Engine::Core

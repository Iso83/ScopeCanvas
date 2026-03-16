#pragma once

#include <vector>

#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"
#include "ScopeCanvasEngineCore/Ids/TypeIds.h"

namespace ScopeCanvas::Engine::Core
{
struct Connector
{
    CanvasConnectorId id{};
    CanvasNodeId nodeId{};
    ConnectorTypeId typeId{};
    std::vector<CanvasEdgeId> edges{};
};
} // namespace ScopeCanvas::Engine::Core

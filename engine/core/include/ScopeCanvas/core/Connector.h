#pragma once

#include <vector>

#include "ids/CanvasIds.h"
#include "ids/TypeIds.h"

namespace ScopeCanvas::Core
{
struct Connector
{
    CanvasConnectorId id{};
    CanvasNodeId nodeId{};
    ConnectorTypeId typeId{};
    std::vector<CanvasEdgeId> edges{};
};
} // namespace ScopeCanvas::Core

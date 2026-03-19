#pragma once

#include "ids/CanvasIds.h"
#include "ids/TypeIds.h"

#include <vector>

namespace ScopeCanvas::Core {
struct Connector {
    CanvasConnectorId id{};
    CanvasNodeId nodeId{};
    ConnectorTypeId typeId{};
    std::vector<CanvasEdgeId> edges{};
};
} // namespace ScopeCanvas::Core

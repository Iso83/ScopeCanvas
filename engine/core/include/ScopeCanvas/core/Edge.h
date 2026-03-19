#pragma once

#include "ids/CanvasIds.h"

namespace ScopeCanvas::Core {
struct Edge {
    CanvasEdgeId id{};
    CanvasConnectorId fromConnector{};
    CanvasConnectorId toConnector{};
};
} // namespace ScopeCanvas::Core

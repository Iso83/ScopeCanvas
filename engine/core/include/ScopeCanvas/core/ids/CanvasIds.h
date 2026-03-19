#pragma once

#include <ScopeCanvas/core/ids/StrongId.h>

namespace ScopeCanvas::Core {
struct CanvasNodeIdTag;
struct CanvasConnectorIdTag;
struct CanvasEdgeIdTag;

using CanvasNodeId = StrongId<CanvasNodeIdTag>;
using CanvasConnectorId = StrongId<CanvasConnectorIdTag>;
using CanvasEdgeId = StrongId<CanvasEdgeIdTag>;
} // namespace ScopeCanvas::Core

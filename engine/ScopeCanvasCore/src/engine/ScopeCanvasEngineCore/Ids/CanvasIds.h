#pragma once

#include "ScopeCanvasEngineCore/Ids/StrongId.h"

namespace ScopeCanvas::Engine::Core
{
struct CanvasNodeIdTag;
struct CanvasConnectorIdTag;
struct CanvasEdgeIdTag;

using CanvasNodeId = StrongId<CanvasNodeIdTag>;
using CanvasConnectorId = StrongId<CanvasConnectorIdTag>;
using CanvasEdgeId = StrongId<CanvasEdgeIdTag>;
} // namespace ScopeCanvas::Engine::Core

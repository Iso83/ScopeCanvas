#pragma once

#include "ScopeCanvasEngineCore/Ids/StrongId.h"

namespace ScopeCanvas::Engine::Core
{
struct NodeTypeIdTag;
struct ConnectorTypeIdTag;
struct LayoutGroupIdTag;

using NodeTypeId = StrongId<NodeTypeIdTag>;
using ConnectorTypeId = StrongId<ConnectorTypeIdTag>;
using LayoutGroupId = StrongId<LayoutGroupIdTag>;
} // namespace ScopeCanvas::Engine::Core

#pragma once

#include <ScopeCanvas/core/ids/StrongId.h>

namespace ScopeCanvas::Core {
struct NodeTypeIdTag;
struct ConnectorTypeIdTag;
struct LayoutGroupIdTag;

using NodeTypeId = StrongId<NodeTypeIdTag>;
using ConnectorTypeId = StrongId<ConnectorTypeIdTag>;
using LayoutGroupId = StrongId<LayoutGroupIdTag>;
} // namespace ScopeCanvas::Core

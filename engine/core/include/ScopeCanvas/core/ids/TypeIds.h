#pragma once

#include <ScopeCanvas/core/ids/StrongId.h>

namespace ScopeCanvas::Core::Ids {
struct NodeTypeIdTag;
struct ConnectorTypeIdTag;
struct EdgeTypeIdTag;

using NodeTypeId = StrongId<NodeTypeIdTag>;
using ConnectorTypeId = StrongId<ConnectorTypeIdTag>;
using EdgeTypeId = StrongId<EdgeTypeIdTag>;
} // namespace ScopeCanvas::Core::Ids

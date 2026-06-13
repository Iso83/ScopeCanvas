#pragma once

#include <ScopeCanvas/core/ids/StrongId.h>

namespace ScopeCanvas::Core::Ids {
struct NodeIdTag;
struct ConnectorIdTag;
struct EdgeIdTag;

using NodeId = StrongId<NodeIdTag>;
using ConnectorId = StrongId<ConnectorIdTag>;
using EdgeId = StrongId<EdgeIdTag>;
} // namespace ScopeCanvas::Core::Ids

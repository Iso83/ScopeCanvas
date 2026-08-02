#pragma once

#include <ScopeCanvas/engine/core/ids/StrongId.h>

namespace ScopeCanvas::Engine::Core::Ids {
struct NodeIdTag;
struct ConnectorIdTag;
struct EdgeIdTag;

using NodeId = StrongId<NodeIdTag>;
using ConnectorId = StrongId<ConnectorIdTag>;
using EdgeId = StrongId<EdgeIdTag>;
} // namespace ScopeCanvas::Engine::Core::Ids

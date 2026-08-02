#pragma once

#include <ScopeCanvas/engine/core/ids/CanvasIds.h>
#include <ScopeCanvas/engine/core/ids/TypeIds.h>
#include <vector>

namespace ScopeCanvas::Engine::Core {
struct Connector {
    Ids::ConnectorId id{};
    Ids::NodeId nodeId{};
    Ids::ConnectorTypeId typeId{};
    std::vector<Ids::EdgeId> edges{};
};
} // namespace ScopeCanvas::Engine::Core

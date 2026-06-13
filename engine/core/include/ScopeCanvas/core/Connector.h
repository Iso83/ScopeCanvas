#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/TypeIds.h>
#include <vector>

namespace ScopeCanvas::Core {
struct Connector {
    Ids::ConnectorId id{};
    Ids::NodeId nodeId{};
    Ids::ConnectorTypeId typeId{};
    std::vector<Ids::EdgeId> edges{};
};
} // namespace ScopeCanvas::Core

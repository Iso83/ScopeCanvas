#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>

namespace ScopeCanvas::Core {
struct Edge {
    Ids::EdgeId id{};
    Ids::ConnectorId fromConnector{};
    Ids::ConnectorId toConnector{};
};
} // namespace ScopeCanvas::Core

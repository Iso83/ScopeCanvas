#pragma once

#include <ScopeCanvas/engine/core/ids/CanvasIds.h>

namespace ScopeCanvas::Engine::Core {
struct Edge {
    Ids::EdgeId id{};
    Ids::ConnectorId fromConnector{};
    Ids::ConnectorId toConnector{};
};
} // namespace ScopeCanvas::Engine::Core

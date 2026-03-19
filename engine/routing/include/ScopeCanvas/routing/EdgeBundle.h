#pragma once

#include <ScopeCanvas/core/Vec2.h>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/StrongId.h>
#include <vector>

namespace ScopeCanvas::Routing {
struct EdgeBundleIdTag;
using EdgeBundleId = Core::StrongId<EdgeBundleIdTag>;

struct EdgeBundle {
    EdgeBundleId id{};
    Core::Vec2 position{};
    std::vector<Core::CanvasEdgeId> edges{};
};
} // namespace ScopeCanvas::Routing

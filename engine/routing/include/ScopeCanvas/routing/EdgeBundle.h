#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/StrongId.h>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Routing {
struct EdgeBundleIdTag;
using EdgeBundleId = Core::StrongId<EdgeBundleIdTag>;

struct EdgeBundle {
    EdgeBundleId id{};
    glm::vec2 position{};
    std::vector<Core::CanvasEdgeId> edges{};
};
} // namespace ScopeCanvas::Routing

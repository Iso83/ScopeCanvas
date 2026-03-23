#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Routing {
struct EdgeRoute {
    Core::CanvasEdgeId edgeId{};
    std::vector<glm::vec2> points{};
    bool preferStraightSegments{false};
};
} // namespace ScopeCanvas::Routing

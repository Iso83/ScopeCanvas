#pragma once

#include <ScopeCanvas/core/Vec2.h>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <vector>

namespace ScopeCanvas::Routing {
struct EdgeRoute {
    Core::CanvasEdgeId edgeId{};
    std::vector<Core::Vec2> points{};
};
} // namespace ScopeCanvas::Routing

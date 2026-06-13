#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Routing {
struct EdgeRoute {
    Core::Ids::EdgeId edgeId{};
    std::vector<glm::vec2> points{};
};
} // namespace ScopeCanvas::Routing

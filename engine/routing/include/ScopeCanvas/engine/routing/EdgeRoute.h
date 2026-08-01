#pragma once

#include <ScopeCanvas/engine/core/ids/CanvasIds.h>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Engine::Routing {
struct EdgeRoute {
    Core::Ids::EdgeId edgeId{};
    std::vector<glm::vec2> points{};
};
} // namespace ScopeCanvas::Routing

#pragma once

#include <vector>

#include "ScopeCanvasRouting/Routing/EdgeRoute.h"

namespace ScopeCanvas::RenderGL::Renderers
{
class EdgeRenderer
{
public:
    [[nodiscard]] std::vector<Engine::Core::Vec2> buildEdgeGeometry(const Engine::Routing::EdgeRoute& route) const;
};
} // namespace ScopeCanvas::RenderGL::Renderers

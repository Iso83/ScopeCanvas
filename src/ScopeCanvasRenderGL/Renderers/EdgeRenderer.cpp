#include "Renderers/EdgeRenderer.h"

namespace ScopeCanvas::RenderGL::Renderers
{
std::vector<Engine::Core::Vec2> EdgeRenderer::buildEdgeGeometry(const Engine::Routing::EdgeRoute& route) const
{
    return route.points;
}
} // namespace ScopeCanvas::RenderGL::Renderers

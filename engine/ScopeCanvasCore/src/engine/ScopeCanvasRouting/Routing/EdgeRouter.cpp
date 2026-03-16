#include "ScopeCanvasRouting/Routing/EdgeRouter.h"

namespace ScopeCanvas::Engine::Routing
{
EdgeRoute EdgeRouter::routeEdge(const Core::DiagramModel& model, Core::CanvasEdgeId edgeId)
{
    (void)model;

    EdgeRoute route{};
    route.edgeId = edgeId;
    return route;
}

std::vector<EdgeRoute> EdgeRouter::routeAll(const Core::DiagramModel& model)
{
    (void)model;
    return {};
}

void EdgeRouter::bundleEdges(std::vector<EdgeRoute>& routes)
{
    (void)routes;
}
} // namespace ScopeCanvas::Engine::Routing

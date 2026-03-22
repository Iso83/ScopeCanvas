#include <ScopeCanvas/routing/EdgeRouter.h>

namespace ScopeCanvas::Routing {
EdgeRoute EdgeRouter::routeEdge(const Core::GraphDocument& model, Core::CanvasEdgeId edgeId) {
    (void)model;

    EdgeRoute route{};
    route.edgeId = edgeId;
    return route;
}

std::vector<EdgeRoute> EdgeRouter::routeAll(const Core::GraphDocument& model) {
    (void)model;
    return {};
}

void EdgeRouter::bundleEdges(std::vector<EdgeRoute>& routes) {
    (void)routes;
}
} // namespace ScopeCanvas::Routing

#include <vector>

#include "ScopeCanvasRouting/Routing/EdgeBundle.h"
#include "ScopeCanvasRouting/Routing/EdgeRoute.h"
#include "ScopeCanvasRouting/Routing/EdgeRouter.h"

int main()
{
    using namespace ScopeCanvas::Engine;

    Routing::EdgeRoute route{};
    route.edgeId = Core::CanvasEdgeId{1};
    route.points.push_back(Core::Vec2{1.0F, 2.0F});

    Routing::EdgeBundle bundle{};
    bundle.id = Routing::EdgeBundleId{1};
    bundle.position = Core::Vec2{3.0F, 4.0F};
    bundle.edges.push_back(route.edgeId);
    if (bundle.edges.empty())
    {
        return 1;
    }

    Core::DiagramModel model{};
    Routing::EdgeRouter router{};

    const Routing::EdgeRoute routedEdge = router.routeEdge(model, Core::CanvasEdgeId{2});
    if (routedEdge.edgeId != Core::CanvasEdgeId{2})
    {
        return 1;
    }

    std::vector<Routing::EdgeRoute> allRoutes = router.routeAll(model);
    router.bundleEdges(allRoutes);

    return 0;
}

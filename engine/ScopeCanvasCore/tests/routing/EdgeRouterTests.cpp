#include <vector>

#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"
#include "ScopeCanvasRouting/Routing/EdgeRouter.h"

int main()
{
    using namespace ScopeCanvas::Engine;

    Core::DiagramModel model{};
    Routing::EdgeRouter router{};

    const Routing::EdgeRoute route = router.routeEdge(model, Core::CanvasEdgeId{1});
    if (route.edgeId != Core::CanvasEdgeId{1})
    {
        return 1;
    }

    std::vector<Routing::EdgeRoute> routes = router.routeAll(model);
    router.bundleEdges(routes);

    return routes.empty() ? 0 : 1;
}

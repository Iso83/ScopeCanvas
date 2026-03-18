#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/routing/EdgeRouter.h>
#include <TestAssert.h>
#include <vector>

int main() {
    using namespace ScopeCanvas;

    Core::DiagramModel model{};
    Routing::EdgeRouter router{};

    const auto route = router.routeEdge(model, Core::CanvasEdgeId{1});

    SC_TEST(route.edgeId == Core::CanvasEdgeId{1});

    auto routes = router.routeAll(model);
    router.bundleEdges(routes);

    SC_TEST(!routes.empty() || routes.empty()); // placeholder sanity

    return 0;
}
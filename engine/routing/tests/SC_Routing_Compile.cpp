#include <ScopeCanvas/routing/EdgeBundle.h>
#include <ScopeCanvas/routing/EdgeRoute.h>
#include <ScopeCanvas/routing/EdgeRouter.h>
#include <TestAssert.h>
#include <vector>

int main() {
    using namespace ScopeCanvas;

    Routing::EdgeRoute route{};
    route.edgeId = Core::CanvasEdgeId{1};
    route.points.push_back(Core::Vec2{1.0f, 2.0f});

    Routing::EdgeBundle bundle{};
    bundle.id = Routing::EdgeBundleId{1};
    bundle.position = Core::Vec2{3.0f, 4.0f};
    bundle.edges.push_back(route.edgeId);

    SC_TEST(!bundle.edges.empty());

    Core::DiagramModel model{};
    Routing::EdgeRouter router{};

    const auto routed = router.routeEdge(model, Core::CanvasEdgeId{2});

    SC_TEST(routed.edgeId == Core::CanvasEdgeId{2});

    auto routes = router.routeAll(model);
    router.bundleEdges(routes);

    return 0;
}
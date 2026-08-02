#include "TestAssert.h"

#include <ScopeCanvas/engine/core/Connector.h>
#include <ScopeCanvas/engine/core/Edge.h>
#include <ScopeCanvas/engine/core/Node.h>
#include <ScopeCanvas/engine/core/ids/CanvasIds.h>
#include <ScopeCanvas/engine/routing/EdgeRouter.h>
#include <ScopeCanvas/engine/routing/IGraphView.h>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {
using namespace ScopeCanvas::Engine::Core;
using namespace ScopeCanvas::Engine::Core::Ids;
using namespace ScopeCanvas::Engine::Routing;

class TestGraphView final : public IGraphView {
public:
    std::unordered_map<std::uint32_t, Node> nodes{};
    std::unordered_map<std::uint32_t, Connector> connectors{};
    std::unordered_map<std::uint32_t, Edge> edges{};
    std::vector<EdgeId> edgeOrder{};

    void forEachEdgeId(const std::function<void(EdgeId)>& f) const override {
        for (EdgeId edgeId : edgeOrder)
            f(edgeId);
    }

    glm::vec2 connectorWorld(const Node& node, std::size_t index) const override {
        const bool output = (index % 2U) == 1U;
        return {output ? node.position.x + node.size.x : node.position.x, node.position.y + 10.0F};
    }

    const Edge* getEdge(EdgeId id) const override {
        const auto it = edges.find(id.value());
        return it == edges.end() ? nullptr : &it->second;
    }

    const Connector* getConnector(ConnectorId id) const override {
        const auto it = connectors.find(id.value());
        return it == connectors.end() ? nullptr : &it->second;
    }

    const Node* getNode(NodeId id) const override {
        const auto it = nodes.find(id.value());
        return it == nodes.end() ? nullptr : &it->second;
    }
};

TestGraphView makeRightToLeftGraph() {
    TestGraphView graph{};

    Node from{};
    from.id = NodeId{1};
    from.position = {220.0F, 100.0F};
    from.size = {100.0F, 60.0F};
    from.connectors = {ConnectorId{1}, ConnectorId{2}};

    Node to{};
    to.id = NodeId{2};
    to.position = {0.0F, 0.0F};
    to.size = {100.0F, 60.0F};
    to.connectors = {ConnectorId{3}, ConnectorId{4}};

    graph.nodes.emplace(from.id.value(), from);
    graph.nodes.emplace(to.id.value(), to);
    graph.connectors.emplace(1U, Connector{ConnectorId{1}, NodeId{1}, {}, {EdgeId{1}}});
    graph.connectors.emplace(4U, Connector{ConnectorId{4}, NodeId{2}, {}, {EdgeId{1}}});
    graph.edges.emplace(1U, Edge{EdgeId{1}, ConnectorId{1}, ConnectorId{4}});
    graph.edgeOrder.push_back(EdgeId{1});
    return graph;
}

TestGraphView makeTwoNodeGraph(float toX = 220.0F) {
    TestGraphView graph{};

    Node from{};
    from.id = NodeId{1};
    from.position = {0.0F, 0.0F};
    from.size = {100.0F, 60.0F};
    from.connectors = {ConnectorId{1}, ConnectorId{2}};

    Node to{};
    to.id = NodeId{2};
    to.position = {toX, 0.0F};
    to.size = {100.0F, 60.0F};
    to.connectors = {ConnectorId{3}, ConnectorId{4}};

    graph.nodes.emplace(from.id.value(), from);
    graph.nodes.emplace(to.id.value(), to);
    graph.connectors.emplace(2U, Connector{ConnectorId{2}, NodeId{1}, {}, {EdgeId{1}}});
    graph.connectors.emplace(3U, Connector{ConnectorId{3}, NodeId{2}, {}, {EdgeId{1}}});
    graph.edges.emplace(1U, Edge{EdgeId{1}, ConnectorId{2}, ConnectorId{3}});
    graph.edgeOrder.push_back(EdgeId{1});
    return graph;
}

int test_null_view_returns_no_routes() {
    const EdgeRouter router{};
    const std::vector<EdgeRoute> routes = router.routeAll(nullptr);
    CPPTEST_ASSERT(routes.empty());
    return 0;
}

int test_valid_edge_creates_route() {
    TestGraphView graph = makeTwoNodeGraph();
    const EdgeRouter router{};

    const std::vector<EdgeRoute> routes = router.routeAll(&graph);

    CPPTEST_ASSERT(routes.size() == 1U);
    CPPTEST_ASSERT(routes[0].edgeId == EdgeId{1});
    CPPTEST_ASSERT(routes[0].points.size() >= 2U);
    CPPTEST_ASSERT(routes[0].points.front().x == 100.0F);
    CPPTEST_ASSERT(routes[0].points.back().x == 220.0F);
    return 0;
}

int test_missing_connector_skips_edge() {
    TestGraphView graph = makeTwoNodeGraph();
    graph.connectors.erase(3U);
    const EdgeRouter router{};

    const std::vector<EdgeRoute> routes = router.routeAll(&graph);

    CPPTEST_ASSERT(routes.empty());
    return 0;
}

int test_close_opposing_ports_add_detour() {
    TestGraphView graph = makeTwoNodeGraph(130.0F);
    const EdgeRouter router{};

    const std::vector<EdgeRoute> routes = router.routeAll(&graph);

    CPPTEST_ASSERT(routes.size() == 1U);
    CPPTEST_ASSERT(routes[0].points.size() == 6U);
    CPPTEST_ASSERT(routes[0].points[2].y > 60.0F);
    CPPTEST_ASSERT(routes[0].points[3].y == routes[0].points[2].y);
    return 0;
}

int test_right_to_left_opposing_ports_uses_direct_route() {
    TestGraphView graph = makeRightToLeftGraph();
    const EdgeRouter router{};

    const std::vector<EdgeRoute> routes = router.routeAll(&graph);

    CPPTEST_ASSERT(routes.size() == 1U);
    CPPTEST_ASSERT(routes[0].points.size() == 2U);
    CPPTEST_ASSERT(routes[0].points.front().x == 220.0F);
    CPPTEST_ASSERT(routes[0].points.back().x == 100.0F);
    return 0;
}
} // namespace

int main() {
    CPPTEST_RUN(test_null_view_returns_no_routes);
    CPPTEST_RUN(test_valid_edge_creates_route);
    CPPTEST_RUN(test_missing_connector_skips_edge);
    CPPTEST_RUN(test_close_opposing_ports_add_detour);
    CPPTEST_RUN(test_right_to_left_opposing_ports_uses_direct_route);
    return 0;
}

#include <ScopeCanvas/core/Connector.h>
#include <ScopeCanvas/core/Edge.h>
#include <ScopeCanvas/core/Node.h>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/routing/EdgeRouter.h>
#include <ScopeCanvas/routing/IGraphView.h>
#include <TestAssert.h>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {
using namespace ScopeCanvas::Core;
using namespace ScopeCanvas::Core::Ids;
using namespace ScopeCanvas::Routing;

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
    SC_TEST(routes.empty());
    return 0;
}

int test_valid_edge_creates_route() {
    TestGraphView graph = makeTwoNodeGraph();
    const EdgeRouter router{};

    const std::vector<EdgeRoute> routes = router.routeAll(&graph);

    SC_TEST(routes.size() == 1U);
    SC_TEST(routes[0].edgeId == EdgeId{1});
    SC_TEST(routes[0].points.size() >= 2U);
    SC_TEST(routes[0].points.front().x == 100.0F);
    SC_TEST(routes[0].points.back().x == 220.0F);
    return 0;
}

int test_missing_connector_skips_edge() {
    TestGraphView graph = makeTwoNodeGraph();
    graph.connectors.erase(3U);
    const EdgeRouter router{};

    const std::vector<EdgeRoute> routes = router.routeAll(&graph);

    SC_TEST(routes.empty());
    return 0;
}

int test_close_opposing_ports_add_detour() {
    TestGraphView graph = makeTwoNodeGraph(130.0F);
    const EdgeRouter router{};

    const std::vector<EdgeRoute> routes = router.routeAll(&graph);

    SC_TEST(routes.size() == 1U);
    SC_TEST(routes[0].points.size() == 6U);
    SC_TEST(routes[0].points[2].y > 60.0F);
    SC_TEST(routes[0].points[3].y == routes[0].points[2].y);
    return 0;
}
} // namespace

int main() {
    SC_RUN_TEST(test_null_view_returns_no_routes);
    SC_RUN_TEST(test_valid_edge_creates_route);
    SC_RUN_TEST(test_missing_connector_skips_edge);
    SC_RUN_TEST(test_close_opposing_ports_add_detour);
    return 0;
}

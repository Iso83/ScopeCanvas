#include <ScopeCanvas/core/DiagramModel.h>
#include <TestAssert.h>

using namespace ScopeCanvas::Core;

static int test_connect() {
    DiagramModel model{};

    const auto n1 = model.createNode(NodeTypeId{1});
    const auto n2 = model.createNode(NodeTypeId{2});

    SC_TEST(n1.isValid());
    SC_TEST(n2.isValid());

    Node* node1 = model.getNode(n1);
    Node* node2 = model.getNode(n2);

    SC_TEST(node1 && node2);
    SC_TEST(!node1->connectors.empty());
    SC_TEST(!node2->connectors.empty());

    const auto c1 = node1->connectors.front();
    const auto c2 = node2->connectors.front();

    const auto edge = model.connect(c1, c2);

    SC_TEST(edge.isValid());
    SC_TEST(model.getEdge(edge) != nullptr);

    return 0;
}

static int test_disconnect() {
    DiagramModel model{};

    const auto n1 = model.createNode(NodeTypeId{1});
    const auto n2 = model.createNode(NodeTypeId{2});

    auto c1 = model.getNode(n1)->connectors.front();
    auto c2 = model.getNode(n2)->connectors.front();

    const auto edge = model.connect(c1, c2);

    model.disconnect(edge);

    SC_TEST(model.getEdge(edge) == nullptr);

    return 0;
}

static int test_grouping() {
    DiagramModel model{};

    const auto n = model.createNode(NodeTypeId{1});
    const LayoutGroupId g{100};

    model.addNodeToGroup(n, g);

    SC_TEST(!model.getNode(n)->groups.empty());
    SC_TEST(!model.getLayoutGroup(g)->nodes.empty());

    model.removeNodeFromGroup(n, g);

    SC_TEST(model.getNode(n)->groups.empty());
    SC_TEST(model.getLayoutGroup(g)->nodes.empty());

    return 0;
}

int main() {
    SC_TEST(test_connect() == 0);
    SC_TEST(test_disconnect() == 0);
    SC_TEST(test_grouping() == 0);

    return 0;
}
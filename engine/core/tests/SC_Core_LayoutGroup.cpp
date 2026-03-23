#include <ScopeCanvas/core/GraphDocument.h>
#include <TestAssert.h>

int main() {
    using namespace ScopeCanvas::Core;

    GraphDocument model{};
    const auto node = model.createNode(NodeTypeId{1});

    const LayoutGroupId g1{100};
    const LayoutGroupId g2{200};

    model.addNodeToGroup(node, g1);
    model.addNodeToGroup(node, g2);

    const Node* n = model.getNode(node);
    const LayoutGroup* group1 = model.getLayoutGroup(g1);
    const LayoutGroup* group2 = model.getLayoutGroup(g2);

    SC_TEST(n && group1 && group2);
    SC_TEST(n->groups.size() == 2);
    SC_TEST(group1->nodes.size() == 1);
    SC_TEST(group2->nodes.size() == 1);

    model.removeNodeFromGroup(node, g1);

    SC_TEST(n->groups.size() == 1);
    SC_TEST(group1->nodes.empty());
    SC_TEST(group2->nodes.size() == 1);
    SC_TEST(group2->nodes.front() == node);

    return 0;
}
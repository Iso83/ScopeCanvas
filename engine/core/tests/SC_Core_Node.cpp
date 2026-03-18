#include <ScopeCanvas/core/Node.h>
#include <TestAssert.h>

int main() {
    using namespace ScopeCanvas::Core;

    Node node{};
    node.id = CanvasNodeId{1};
    node.typeId = NodeTypeId{10};
    node.setPosition(Vec2{5.0f, 7.0f});
    node.setSize(Vec2{20.0f, 30.0f});
    node.groups.push_back(LayoutGroupId{100});

    SC_TEST(node.id.isValid());
    SC_TEST(node.typeId.isValid());
    SC_TEST((node.position == Vec2{5.0f, 7.0f}));
    SC_TEST((node.size == Vec2{20.0f, 30.0f}));
    SC_TEST(!node.groups.empty());

    return 0;
}
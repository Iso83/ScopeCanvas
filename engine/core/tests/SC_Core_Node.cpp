#include <ScopeCanvas/core/Node.h>
#include <TestAssert.h>

int main() {
    using namespace ScopeCanvas::Core;

    Node node{};
    node.id = CanvasNodeId{1};
    node.typeId = NodeTypeId{10};
    node.setPosition(glm::vec2{5.0f, 7.0f});
    node.setSize(glm::vec2{20.0f, 30.0f});
    node.groups.push_back(LayoutGroupId{100});

    SC_TEST(node.id.isValid());
    SC_TEST(node.typeId.isValid());
    SC_TEST((node.position == glm::vec2{5.0f, 7.0f}));
    SC_TEST((node.size == glm::vec2{20.0f, 30.0f}));
    SC_TEST(!node.groups.empty());

    return 0;
}
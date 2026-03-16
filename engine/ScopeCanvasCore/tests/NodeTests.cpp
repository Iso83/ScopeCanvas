#include "ScopeCanvasEngineCore/Core/Node.h"

int main()
{
    using namespace ScopeCanvas::Engine::Core;

    Node node{};
    node.id = CanvasNodeId{1};
    node.typeId = NodeTypeId{10};
    node.setPosition(Vec2{5.0F, 7.0F});
    node.setSize(Vec2{20.0F, 30.0F});
    node.groups.push_back(LayoutGroupId{100});

    return (node.id.isValid() && node.typeId.isValid() && node.position == Vec2{5.0F, 7.0F} &&
            node.size == Vec2{20.0F, 30.0F} && !node.groups.empty())
               ? 0
               : 1;
}

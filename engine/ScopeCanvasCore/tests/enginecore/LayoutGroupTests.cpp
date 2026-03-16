#include "ScopeCanvasEngineCore/Core/DiagramModel.h"

int main()
{
    using namespace ScopeCanvas::Engine::Core;

    DiagramModel model{};
    const CanvasNodeId nodeId = model.createNode(NodeTypeId{1});

    const LayoutGroupId groupA{100};
    const LayoutGroupId groupB{200};

    model.addNodeToGroup(nodeId, groupA);
    model.addNodeToGroup(nodeId, groupB);

    const Node* nodeWithBothGroups = model.getNode(nodeId);
    const LayoutGroup* firstGroup = model.getLayoutGroup(groupA);
    const LayoutGroup* secondGroup = model.getLayoutGroup(groupB);

    if (nodeWithBothGroups == nullptr || firstGroup == nullptr || secondGroup == nullptr)
    {
        return 1;
    }

    if (nodeWithBothGroups->groups.size() != 2 || firstGroup->nodes.size() != 1 || secondGroup->nodes.size() != 1)
    {
        return 1;
    }

    model.removeNodeFromGroup(nodeId, groupA);

    const Node* nodeAfterRemoval = model.getNode(nodeId);
    const LayoutGroup* firstGroupAfterRemoval = model.getLayoutGroup(groupA);
    const LayoutGroup* secondGroupAfterRemoval = model.getLayoutGroup(groupB);

    if (nodeAfterRemoval == nullptr || firstGroupAfterRemoval == nullptr || secondGroupAfterRemoval == nullptr)
    {
        return 1;
    }

    if (nodeAfterRemoval->groups.size() != 1)
    {
        return 1;
    }

    if (!firstGroupAfterRemoval->nodes.empty())
    {
        return 1;
    }

    if (secondGroupAfterRemoval->nodes.size() != 1 || secondGroupAfterRemoval->nodes.front() != nodeId)
    {
        return 1;
    }

    return 0;
}

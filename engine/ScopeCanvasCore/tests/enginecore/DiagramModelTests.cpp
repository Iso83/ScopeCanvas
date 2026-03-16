#include "ScopeCanvasEngineCore/Core/DiagramModel.h"

int main()
{
    using namespace ScopeCanvas::Engine::Core;

    DiagramModel model{};

    const CanvasNodeId firstNodeId = model.createNode(NodeTypeId{1});
    const CanvasNodeId secondNodeId = model.createNode(NodeTypeId{2});

    if (!firstNodeId.isValid() || !secondNodeId.isValid())
    {
        return 1;
    }

    Node* firstNode = model.getNode(firstNodeId);
    Node* secondNode = model.getNode(secondNodeId);
    if (firstNode == nullptr || secondNode == nullptr || firstNode->connectors.empty() || secondNode->connectors.empty())
    {
        return 1;
    }

    const CanvasConnectorId firstConnectorId = firstNode->connectors.front();
    const CanvasConnectorId secondConnectorId = secondNode->connectors.front();

    const CanvasEdgeId edgeId = model.connect(firstConnectorId, secondConnectorId);
    if (!edgeId.isValid() || model.getEdge(edgeId) == nullptr)
    {
        return 1;
    }

    const Connector* firstConnectorAfterConnect = model.getConnector(firstConnectorId);
    const Connector* secondConnectorAfterConnect = model.getConnector(secondConnectorId);
    if (firstConnectorAfterConnect == nullptr || secondConnectorAfterConnect == nullptr ||
        firstConnectorAfterConnect->edges.empty() || secondConnectorAfterConnect->edges.empty())
    {
        return 1;
    }

    model.disconnect(edgeId);
    if (model.getEdge(edgeId) != nullptr)
    {
        return 1;
    }

    const Connector* firstConnectorAfterDisconnect = model.getConnector(firstConnectorId);
    const Connector* secondConnectorAfterDisconnect = model.getConnector(secondConnectorId);
    if (firstConnectorAfterDisconnect == nullptr || secondConnectorAfterDisconnect == nullptr ||
        !firstConnectorAfterDisconnect->edges.empty() || !secondConnectorAfterDisconnect->edges.empty())
    {
        return 1;
    }

    const LayoutGroupId groupId{11};
    model.addNodeToGroup(firstNodeId, groupId);

    const Node* groupedNode = model.getNode(firstNodeId);
    const LayoutGroup* group = model.getLayoutGroup(groupId);
    if (groupedNode == nullptr || group == nullptr || groupedNode->groups.empty() || group->nodes.empty())
    {
        return 1;
    }

    model.removeNodeFromGroup(firstNodeId, groupId);

    const Node* ungroupedNode = model.getNode(firstNodeId);
    const LayoutGroup* sameGroup = model.getLayoutGroup(groupId);
    if (ungroupedNode == nullptr || sameGroup == nullptr || !ungroupedNode->groups.empty() || !sameGroup->nodes.empty())
    {
        return 1;
    }

    return 0;
}

#include "ScopeCanvasEngineCore/Core/DiagramModel.h"
#include "ScopeCanvasEngineCore/Logic/ConnectorLogic.h"

int main()
{
    using namespace ScopeCanvas::Engine::Core;

    DiagramModel model{};

    const CanvasNodeId firstNodeId = model.createNode(NodeTypeId{1});
    const CanvasNodeId secondNodeId = model.createNode(NodeTypeId{2});

    const Node* firstNode = model.getNode(firstNodeId);
    const Node* secondNode = model.getNode(secondNodeId);
    if (firstNode == nullptr || secondNode == nullptr || firstNode->connectors.size() < 2 || secondNode->connectors.empty())
    {
        return 1;
    }

    const CanvasConnectorId firstNodeConnectorA = firstNode->connectors[0];
    const CanvasConnectorId firstNodeConnectorB = firstNode->connectors[1];
    const CanvasConnectorId secondNodeConnector = secondNode->connectors[0];

    if (!ConnectorLogic::sameNode(model, firstNodeConnectorA, firstNodeConnectorB))
    {
        return 1;
    }

    const CanvasEdgeId edgeId = model.connect(firstNodeConnectorA, secondNodeConnector);
    if (!edgeId.isValid())
    {
        return 1;
    }

    if (!ConnectorLogic::edgeExists(model, firstNodeConnectorA, secondNodeConnector))
    {
        return 1;
    }

    if (ConnectorLogic::canConnect(model, firstNodeConnectorA, secondNodeConnector))
    {
        return 1;
    }

    if (ConnectorLogic::canConnect(model, firstNodeConnectorA, firstNodeConnectorA))
    {
        return 1;
    }

    return 0;
}

#include "ScopeCanvasEngineCore/Core/Connector.h"
#include "ScopeCanvasEngineCore/Core/Edge.h"

int main()
{
    using namespace ScopeCanvas::Engine::Core;

    Connector connector{};
    connector.id = CanvasConnectorId{2};
    connector.nodeId = CanvasNodeId{1};
    connector.typeId = ConnectorTypeId{11};

    const Edge edge{CanvasEdgeId{3}, connector.id, CanvasConnectorId{4}};
    connector.edges.push_back(edge.id);

    return (connector.id.isValid() && connector.nodeId.isValid() && connector.typeId.isValid() &&
            !connector.edges.empty() && connector.edges.front() == edge.id)
               ? 0
               : 1;
}

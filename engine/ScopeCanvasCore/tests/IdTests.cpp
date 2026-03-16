#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"
#include "ScopeCanvasEngineCore/Ids/TypeIds.h"

int main()
{
    using namespace ScopeCanvas::Engine::Core;

    const CanvasNodeId nodeId{1};
    const CanvasConnectorId connectorId{2};
    const CanvasEdgeId edgeId{3};
    const NodeTypeId nodeTypeId{4};
    const ConnectorTypeId connectorTypeId{5};
    const LayoutGroupId layoutGroupId{6};

    return (nodeId.isValid() && connectorId.isValid() && edgeId.isValid() && nodeTypeId.isValid() &&
            connectorTypeId.isValid() && layoutGroupId.isValid())
               ? 0
               : 1;
}

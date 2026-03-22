#include "logic/ConnectorLogic.h"

#include <ScopeCanvas/core/Connector.h>
#include <ScopeCanvas/core/GraphDocument.h>

namespace ScopeCanvas::Core {
bool ConnectorLogic::canConnect(const GraphDocument& model, CanvasConnectorId a, CanvasConnectorId b) {
    if (a == b) {
        return false;
    }

    if (model.getConnector(a) == nullptr || model.getConnector(b) == nullptr) {
        return false;
    }

    if (edgeExists(model, a, b)) {
        return false;
    }

    return true;
}

bool ConnectorLogic::edgeExists(const GraphDocument& model, CanvasConnectorId a, CanvasConnectorId b) {
    const Connector* connector = model.getConnector(a);
    if (connector == nullptr) {
        return false;
    }

    for (const CanvasEdgeId edgeId : connector->edges) {
        const Edge* edge = model.getEdge(edgeId);
        if (edge != nullptr && edge->fromConnector == a && edge->toConnector == b) {
            return true;
        }
    }

    return false;
}

bool ConnectorLogic::sameNode(const GraphDocument& model, CanvasConnectorId a, CanvasConnectorId b) {
    const Connector* connectorA = model.getConnector(a);
    const Connector* connectorB = model.getConnector(b);
    if (connectorA == nullptr || connectorB == nullptr) {
        return false;
    }

    return connectorA->nodeId == connectorB->nodeId;
}
} // namespace ScopeCanvas::Core

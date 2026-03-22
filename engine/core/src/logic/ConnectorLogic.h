#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>

namespace ScopeCanvas::Core {
class GraphDocument;

class ConnectorLogic {
  public:
    static bool canConnect(const GraphDocument& model, CanvasConnectorId a, CanvasConnectorId b);
    static bool edgeExists(const GraphDocument& model, CanvasConnectorId a, CanvasConnectorId b);
    static bool sameNode(const GraphDocument& model, CanvasConnectorId a, CanvasConnectorId b);
};
} // namespace ScopeCanvas::Core

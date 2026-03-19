#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>

namespace ScopeCanvas::Core {
class DiagramModel;

class ConnectorLogic {
  public:
    static bool canConnect(const DiagramModel& model, CanvasConnectorId a, CanvasConnectorId b);
    static bool edgeExists(const DiagramModel& model, CanvasConnectorId a, CanvasConnectorId b);
    static bool sameNode(const DiagramModel& model, CanvasConnectorId a, CanvasConnectorId b);
};
} // namespace ScopeCanvas::Core

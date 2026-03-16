#pragma once

#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"

namespace ScopeCanvas::Engine::Core
{
class DiagramModel;

class ConnectorLogic
{
public:
    static bool canConnect(const DiagramModel& model, CanvasConnectorId a, CanvasConnectorId b);
    static bool edgeExists(const DiagramModel& model, CanvasConnectorId a, CanvasConnectorId b);
    static bool sameNode(const DiagramModel& model, CanvasConnectorId a, CanvasConnectorId b);
};
} // namespace ScopeCanvas::Engine::Core

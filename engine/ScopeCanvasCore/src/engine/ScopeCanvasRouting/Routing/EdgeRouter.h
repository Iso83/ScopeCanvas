#pragma once

#include <vector>

#include "ScopeCanvasEngineCore/Core/Connector.h"
#include "ScopeCanvasEngineCore/Core/DiagramModel.h"
#include "ScopeCanvasEngineCore/Core/Edge.h"
#include "ScopeCanvasEngineCore/Core/Node.h"
#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"
#include "ScopeCanvasRouting/Routing/EdgeRoute.h"

namespace ScopeCanvas::Engine::Routing
{
class EdgeRouter
{
public:
    EdgeRoute routeEdge(const Core::DiagramModel& model, Core::CanvasEdgeId edgeId);

    std::vector<EdgeRoute> routeAll(const Core::DiagramModel& model);

    void bundleEdges(std::vector<EdgeRoute>& routes);
};
} // namespace ScopeCanvas::Engine::Routing

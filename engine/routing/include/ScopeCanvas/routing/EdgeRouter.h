#pragma once

#include <ScopeCanvas/core/Connector.h>
#include <ScopeCanvas/core/Edge.h>
#include <ScopeCanvas/core/GraphDocument.h>
#include <ScopeCanvas/core/Node.h>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/routing/EdgeRoute.h>
#include <vector>

namespace ScopeCanvas::Routing {
class EdgeRouter {
  public:
    EdgeRoute routeEdge(const Core::GraphDocument& model, Core::CanvasEdgeId edgeId);

    std::vector<EdgeRoute> routeAll(const Core::GraphDocument& model);

    void bundleEdges(std::vector<EdgeRoute>& routes);
};
} // namespace ScopeCanvas::Routing

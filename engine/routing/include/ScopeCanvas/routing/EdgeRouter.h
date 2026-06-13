#pragma once

#include <ScopeCanvas/routing/EdgeRoute.h>
#include <vector>

namespace ScopeCanvas::Routing {
class IGraphView;

class EdgeRouter {
  public:
    [[nodiscard]] std::vector<EdgeRoute> routeAll(const IGraphView* view) const;
};
} // namespace ScopeCanvas::Routing

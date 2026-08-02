#pragma once

#include <ScopeCanvas/engine/routing/EdgeRoute.h>
#include <vector>

namespace ScopeCanvas::Engine::Routing {
class IGraphView;

class EdgeRouter {
public:
    [[nodiscard]] std::vector<EdgeRoute> routeAll(const IGraphView* view) const;
};
} // namespace ScopeCanvas::Engine::Routing

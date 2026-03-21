#pragma once

#include <ScopeCanvas/core/DiagramModel.h>
#include <ScopeCanvas/render/scene/RenderScene.h>
#include <ScopeCanvas/routing/EdgeRoute.h>
#include <vector>

namespace ScopeCanvas::Render::Scene {
class SceneBuilder {
  public:
    [[nodiscard]] RenderScene build(const Core::DiagramModel& model,
                                    const std::vector<Routing::EdgeRoute>& edgeRoutes) const;
};
} // namespace ScopeCanvas::Render::Scene

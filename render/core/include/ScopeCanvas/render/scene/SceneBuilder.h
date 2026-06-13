#pragma once

#include <ScopeCanvas/render/scene/RenderScene.h>
#include <ScopeCanvas/routing/EdgeRoute.h>
#include <vector>

namespace ScopeCanvas::Routing {
class IGraphView;
}

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Render::Scene {
class SceneBuilder {
  public:
    [[nodiscard]] RenderScene build(const Routing::IGraphView& model, const std::vector<Routing::EdgeRoute>& edgeRoutes,
                                    const Camera::Camera2D& camera) const;
};
} // namespace ScopeCanvas::Render::Scene

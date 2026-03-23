#pragma once

#include <ScopeCanvas/core/GraphDocument.h>
#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/render/scene/RenderScene.h>
#include <ScopeCanvas/routing/EdgeRoute.h>
#include <vector>

namespace ScopeCanvas::Render::Scene {
class SceneBuilder {
  public:
    [[nodiscard]] RenderScene build(const Core::GraphDocument& model, const std::vector<Routing::EdgeRoute>& edgeRoutes,
                                    const Camera::Camera2D& camera) const;
};
} // namespace ScopeCanvas::Render::Scene

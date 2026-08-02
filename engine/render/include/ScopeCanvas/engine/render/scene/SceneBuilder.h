#pragma once

#include <ScopeCanvas/engine/render/scene/RenderScene.h>
#include <ScopeCanvas/engine/routing/EdgeRoute.h>
#include <vector>

namespace ScopeCanvas::Engine::Routing {
class IGraphView;
}

namespace ScopeCanvas::Engine::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Engine::Render::Scene {
class SceneBuilder {
public:
    [[nodiscard]] RenderScene build(const Routing::IGraphView& model, const std::vector<Routing::EdgeRoute>& edgeRoutes,
                                    const Camera::Camera2D& camera) const;
};
} // namespace ScopeCanvas::Engine::Render::Scene

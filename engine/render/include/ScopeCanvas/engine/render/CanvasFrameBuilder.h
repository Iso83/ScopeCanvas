#pragma once

#include <ScopeCanvas/engine/render/scene/RenderScene.h>
#include <vector>

namespace ScopeCanvas::Engine::Render {
struct CanvasFrameData {
    std::vector<Scene::NodeRenderData> nodes{};
    std::vector<Scene::EdgeRenderData> edges{};
};

class CanvasFrameBuilder {
  public:
    [[nodiscard]] CanvasFrameData buildFrame(const Scene::RenderScene& scene) const;
};
} // namespace ScopeCanvas::Render

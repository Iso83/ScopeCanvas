#pragma once

#include <ScopeCanvas/render/scene/RenderScene.h>
#include <vector>

namespace ScopeCanvas::Render::Renderers {
class NodeRenderer {
  public:
    [[nodiscard]] std::vector<Scene::NodeRenderData> collect(const Scene::RenderScene& scene) const;
};
} // namespace ScopeCanvas::Render::Renderers

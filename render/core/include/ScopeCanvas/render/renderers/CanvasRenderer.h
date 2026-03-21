#pragma once

#include <ScopeCanvas/render/renderers/EdgeRenderer.h>
#include <ScopeCanvas/render/renderers/NodeRenderer.h>
#include <ScopeCanvas/render/scene/RenderScene.h>
#include <vector>

namespace ScopeCanvas::Render::Renderers {
struct CanvasFrameData {
    std::vector<Scene::NodeRenderData> nodes{};
    std::vector<Scene::EdgeRenderData> edges{};
};

class CanvasRenderer {
  public:
    [[nodiscard]] CanvasFrameData buildFrame(const Scene::RenderScene& scene) const;

  private:
    NodeRenderer m_nodeRenderer{};
    EdgeRenderer m_edgeRenderer{};
};
} // namespace ScopeCanvas::Render::Renderers

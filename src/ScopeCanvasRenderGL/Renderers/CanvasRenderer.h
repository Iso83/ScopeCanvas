#pragma once

#include <vector>

#include "Renderers/EdgeRenderer.h"
#include "Renderers/NodeRenderer.h"
#include "Scene/RenderScene.h"

namespace ScopeCanvas::Render::Renderers
{
struct CanvasFrameData
{
    std::vector<Scene::NodeRenderData> nodes{};
    std::vector<Scene::EdgeRenderData> edges{};
};

class CanvasRenderer
{
public:
    [[nodiscard]] CanvasFrameData buildFrame(const Scene::RenderScene& scene) const;

private:
    NodeRenderer m_nodeRenderer{};
    EdgeRenderer m_edgeRenderer{};
};
} // namespace ScopeCanvas::Render::Renderers

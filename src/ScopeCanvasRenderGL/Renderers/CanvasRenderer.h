#pragma once

#include <vector>

#include "Renderers/EdgeRenderer.h"
#include "Renderers/GridRenderer.h"
#include "Renderers/NodeRenderer.h"
#include "Scene/RenderScene.h"

namespace ScopeCanvas::RenderGL::Renderers
{
struct CanvasFrameData
{
    std::vector<Engine::Core::Vec2> gridVertices{};
    std::vector<Scene::NodeRenderData> nodes{};
    std::vector<Scene::EdgeRenderData> edges{};
};

class CanvasRenderer
{
public:
    [[nodiscard]] CanvasFrameData buildFrame(const Scene::RenderScene& scene,
                                             const Camera::Camera2D& camera,
                                             float gridSpacing,
                                             int gridHalfExtent) const;

private:
    GridRenderer m_gridRenderer{};
    NodeRenderer m_nodeRenderer{};
    EdgeRenderer m_edgeRenderer{};
};
} // namespace ScopeCanvas::RenderGL::Renderers

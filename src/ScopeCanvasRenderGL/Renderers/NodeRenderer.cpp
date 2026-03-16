#include "Renderers/NodeRenderer.h"

namespace ScopeCanvas::RenderGL::Renderers
{
std::vector<Scene::NodeRenderData> NodeRenderer::collect(const Scene::RenderScene& scene) const
{
    return scene.nodes;
}
} // namespace ScopeCanvas::RenderGL::Renderers

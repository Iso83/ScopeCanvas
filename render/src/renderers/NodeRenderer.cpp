#include <ScopeCanvas/render/renderers/NodeRenderer.h>

namespace ScopeCanvas::Render::Renderers
{
std::vector<Scene::NodeRenderData> NodeRenderer::collect(const Scene::RenderScene& scene) const
{
    return scene.nodes;
}
} // namespace ScopeCanvas::Render::Renderers

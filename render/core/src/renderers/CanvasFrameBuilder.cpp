#include <ScopeCanvas/render/renderers/CanvasFrameBuilder.h>

namespace ScopeCanvas::Render::Renderers {
CanvasFrameData CanvasFrameBuilder::buildFrame(const Scene::RenderScene& scene) const {
    CanvasFrameData frame{};
    frame.nodes = scene.nodes;
    frame.edges = scene.edges;
    return frame;
}
} // namespace ScopeCanvas::Render::Renderers

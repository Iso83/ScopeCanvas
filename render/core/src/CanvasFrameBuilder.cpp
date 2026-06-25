#include <ScopeCanvas/render/CanvasFrameBuilder.h>

namespace ScopeCanvas::Render {
CanvasFrameData CanvasFrameBuilder::buildFrame(const Scene::RenderScene& scene) const {
    CanvasFrameData frame{};
    frame.nodes = scene.nodes;
    frame.edges = scene.edges;
    return frame;
}
} // namespace ScopeCanvas::Render

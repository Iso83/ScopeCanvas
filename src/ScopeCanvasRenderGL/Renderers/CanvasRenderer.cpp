#include "Renderers/CanvasRenderer.h"

namespace ScopeCanvas::RenderGL::Renderers
{
CanvasFrameData CanvasRenderer::buildFrame(const Scene::RenderScene& scene,
                                           const Camera::Camera2D& camera,
                                           float gridSpacing,
                                           int gridHalfExtent) const
{
    CanvasFrameData frame{};
    frame.gridVertices = m_gridRenderer.buildGridVertices(camera, gridSpacing, gridHalfExtent);
    frame.nodes = m_nodeRenderer.collect(scene);

    frame.edges.reserve(scene.edges.size());
    for (const Scene::EdgeRenderData& edge : scene.edges)
    {
        Scene::EdgeRenderData drawEdge{};
        drawEdge.edgeId = edge.edgeId;

        Engine::Routing::EdgeRoute route{};
        route.edgeId = edge.edgeId;
        route.points = edge.points;

        drawEdge.points = m_edgeRenderer.buildEdgeGeometry(route);
        frame.edges.push_back(drawEdge);
    }

    return frame;
}
} // namespace ScopeCanvas::RenderGL::Renderers

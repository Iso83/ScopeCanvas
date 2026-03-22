#include "ScopeCanvas/render/CanvasRenderer.h"

#include <ScopeCanvas/core/GraphDocument.h>
#include <ScopeCanvas/render/camera/Camera2D.h>
#include <glad/glad.h>

namespace ScopeCanvas::Render {

bool CanvasRenderer::init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return m_grid.init() && m_nodes.init() && m_edges.init() && m_selection.init();
}

void CanvasRenderer::shutdown() {
    m_grid.shutdown();
    m_nodes.shutdown();
    m_edges.shutdown();
    m_selection.shutdown();
}

void CanvasRenderer::render(const Core::GraphDocument& document, const std::vector<Routing::EdgeRoute>& routes,
                            const Camera::Camera2D& camera, const CanvasRenderOptions& options) const {
    const Scene::RenderScene scene = m_sceneBuilder.build(document, routes, camera);

    glClearColor(0.08F, 0.09F, 0.11F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (options.showGrid) {
        m_grid.render(camera, options.gridSize);
    }
    if (options.showEdges) {
        m_edges.render(scene.edges, camera);
    }
    if (options.showNodes) {
        m_nodes.render(scene.nodes, camera, options.selectedNodeId);
    }
    if (options.showConnectors) {
        m_edges.renderConnectors(scene.connectorAnchors, camera);
    }
    if (options.selectionRectActive) {
        m_selection.render(camera.viewProjection(), options.selectionRectStart, options.selectionRectEnd);
    }
}

} // namespace ScopeCanvas::Render

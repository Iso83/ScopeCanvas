#include "Render/Renderer.h"

#include <glad/glad.h>

Renderer::Renderer() : m_viewportWidth(1280), m_viewportHeight(720) {}

bool Renderer::init(const char* gridVertexShaderPath,
                    const char* gridFragmentShaderPath,
                    const char* nodeVertexShaderPath,
                    const char* nodeFragmentShaderPath,
                    const char* edgeVertexShaderPath,
                    const char* edgeFragmentShaderPath,
                    const char* selectionRectVertexShaderPath,
                    const char* selectionRectFragmentShaderPath,
                    int viewportWidth,
                    int viewportHeight) {
    m_viewportWidth = viewportWidth;
    m_viewportHeight = viewportHeight;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_camera.setViewportSize(viewportWidth, viewportHeight);
    if (!m_gridRenderer.init(gridVertexShaderPath, gridFragmentShaderPath)) {
        return false;
    }

    if (!m_edgeRenderer.init(edgeVertexShaderPath, edgeFragmentShaderPath)) {
        return false;
    }

    if (!m_nodeRenderer.init(nodeVertexShaderPath, nodeFragmentShaderPath)) {
        return false;
    }

    return m_selectionRectRenderer.init(selectionRectVertexShaderPath, selectionRectFragmentShaderPath);
}

void Renderer::resize(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
    m_camera.setViewportSize(width, height);
    glViewport(0, 0, width, height);
}

void Renderer::render(const GraphDocument& model, const Viewport& viewport) {
    glClearColor(0.10f, 0.11f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const glm::mat4 viewProjection = m_camera.viewProjection();

    m_gridRenderer.render(viewProjection, m_viewportWidth, m_viewportHeight);
    m_edgeRenderer.renderEdges(model, viewProjection, viewport.hoveredEdgeId);
    m_nodeRenderer.render(model.nodes(), viewProjection);
    m_edgeRenderer.renderConnectors(model.nodes(), viewProjection, viewport.hoveredConnectorId);

    if (viewport.selectionRectActive) {
        m_selectionRectRenderer.render(viewProjection, viewport.selectionRectStart, viewport.selectionRectEnd);
    }

    if (viewport.previewActive) {
        m_edgeRenderer.renderPreviewEdge(model,
                                         viewport.previewStartNode,
                                         viewport.previewStartConnector,
                                         viewport.previewPosition,
                                         viewProjection);
    }
}

#include "Render/Renderer.h"

#include <glad/glad.h>

Renderer::Renderer() : m_viewportWidth(1280), m_viewportHeight(720) {}

bool Renderer::init(const char* gridVertexShaderPath,
                    const char* gridFragmentShaderPath,
                    const char* nodeVertexShaderPath,
                    const char* nodeFragmentShaderPath,
                    const char* edgeVertexShaderPath,
                    const char* edgeFragmentShaderPath,
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

    return m_nodeRenderer.init(nodeVertexShaderPath, nodeFragmentShaderPath);
}

void Renderer::resize(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
    m_camera.setViewportSize(width, height);
    glViewport(0, 0, width, height);
}

void Renderer::render(const DiagramModel& model,
                      bool previewActive,
                      uint32_t previewStartNode,
                      uint32_t previewStartConnector,
                      const glm::vec2& previewPosition) {
    glClearColor(0.10f, 0.11f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const glm::mat4 viewProjection = m_camera.viewProjection();

    m_gridRenderer.render(viewProjection, m_viewportWidth, m_viewportHeight);
    m_edgeRenderer.renderEdges(model, viewProjection);
    m_nodeRenderer.render(model.nodes(), viewProjection);
    m_edgeRenderer.renderConnectors(model.nodes(), viewProjection);

    if (previewActive) {
        m_edgeRenderer.renderPreviewEdge(model,
                                         previewStartNode,
                                         previewStartConnector,
                                         previewPosition,
                                         viewProjection);
    }
}

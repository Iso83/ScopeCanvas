#pragma once

#include "Engine/DiagramModel.h"
#include "Render/EdgeRenderer.h"
#include "Render/GridRenderer.h"
#include "Render/NodeRenderer.h"
#include "Render/SelectionRectRenderer.h"
#include "View/Camera2D.h"

class Renderer {
public:
    Renderer();

    bool init(const char* gridVertexShaderPath,
              const char* gridFragmentShaderPath,
              const char* nodeVertexShaderPath,
              const char* nodeFragmentShaderPath,
              const char* edgeVertexShaderPath,
              const char* edgeFragmentShaderPath,
              const char* selectionRectVertexShaderPath,
              const char* selectionRectFragmentShaderPath,
              int viewportWidth,
              int viewportHeight);
    void resize(int width, int height);
    void render(const DiagramModel& model,
                uint32_t hoveredEdgeId,
                uint32_t hoveredConnectorId,
                bool selectionRectActive = false,
                const glm::vec2& selectionRectStart = glm::vec2(0.0f),
                const glm::vec2& selectionRectEnd = glm::vec2(0.0f),
                bool previewActive = false,
                uint32_t previewStartNode = 0,
                uint32_t previewStartConnector = 0,
                const glm::vec2& previewPosition = glm::vec2(0.0f));

    Camera2D& camera() { return m_camera; }
    int viewportWidth() const { return m_viewportWidth; }
    int viewportHeight() const { return m_viewportHeight; }

private:
    Camera2D m_camera;
    GridRenderer m_gridRenderer;
    EdgeRenderer m_edgeRenderer;
    NodeRenderer m_nodeRenderer;
    SelectionRectRenderer m_selectionRectRenderer;
    int m_viewportWidth;
    int m_viewportHeight;
};

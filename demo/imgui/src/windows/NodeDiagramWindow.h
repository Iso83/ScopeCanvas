#pragma once

#define GLFW_INCLUDE_NONE
#include "diagram/DiagramBasics.h"

#include <GLFW/glfw3.h>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/render/CanvasRenderer.h>
#include <ScopeCanvas/render/camera/Camera2D.h>
#include <string>

namespace ScopeCanvas::Studio {
class NodeDiagramWindow {
  public:
    NodeDiagramWindow(GLFWwindow* window, DiagramBasics* basics, ViewState* viewState, std::string title);
    ~NodeDiagramWindow();

    void draw();
    [[nodiscard]] Core::CanvasEdgeId selectedEdge() const;
    void clearSelectedEdge();

  private:
    GLFWwindow* m_window{};
    DiagramBasics* m_basics{};
    ViewState* m_viewState{};
    std::string m_title{};

    Render::Camera::Camera2D m_camera{};
    Render::CanvasRenderer m_renderer{};
    bool m_rendererInitialized{false};

    unsigned int m_framebuffer{0};
    unsigned int m_colorTexture{0};
    unsigned int m_depthStencilRenderbuffer{0};
    int m_renderWidth{0};
    int m_renderHeight{0};

    Core::CanvasNodeId m_dragNode{};
    glm::vec2 m_dragOffset{};
    glm::vec2 m_dragAnchorStartPosition{};
    std::vector<Core::CanvasNodeId> m_dragSelection{};
    std::vector<glm::vec2> m_dragSelectionStartPositions{};
    Core::CanvasConnectorId m_pendingConnector{};
    bool m_reconnectingEdge{false};
    bool m_reconnectingFromStart{false};
    Core::CanvasConnectorId m_reconnectFixedConnector{};
    Core::CanvasConnectorId m_reconnectOriginalFrom{};
    Core::CanvasConnectorId m_reconnectOriginalTo{};
    Core::CanvasEdgeId m_selectedEdge{};
    Core::CanvasConnectorId m_hoveredConnector{};
    bool m_selectionRectActive{false};
    glm::vec2 m_selectionRectStart{};
    glm::vec2 m_selectionRectEnd{};
    bool m_showGrid{true};
    bool m_showDebug{false};

    void ensureRenderTarget(int width, int height);
    void releaseRenderTarget();
    glm::vec2 screenToWorld(float sx, float sy, float w, float h) const;
    glm::vec2 connectorWorld(const Core::Node& node, std::size_t index) const;
    Core::CanvasNodeId pickNode(const glm::vec2& world) const;
    Core::CanvasConnectorId pickConnector(const glm::vec2& world) const;
    void applySelectionRect();
    glm::vec2 snapToGrid(glm::vec2 position) const;
};
} // namespace ScopeCanvas::Studio

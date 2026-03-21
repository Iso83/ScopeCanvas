#pragma once

#define GLFW_INCLUDE_NONE
#include "diagram/DiagramBasics.h"

#include <GLFW/glfw3.h>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/render/camera/Camera2D.h>
#include <string>

namespace ScopeCanvas::Studio {
class NodeDiagramWindow {
  public:
    NodeDiagramWindow(GLFWwindow* window, DiagramBasics* basics, ViewState* viewState, std::string title);
    ~NodeDiagramWindow();

    void draw();

  private:
    void ensureRenderTarget(int width, int height);
    void releaseRenderTarget();
    glm::vec2 screenToWorld(float sx, float sy, float w, float h) const;

    GLFWwindow* m_window{};
    DiagramBasics* m_basics{};
    ViewState* m_viewState{};
    std::string m_title{};

    Render::Camera::Camera2D m_camera{};

    unsigned int m_framebuffer{0};
    unsigned int m_colorTexture{0};
    unsigned int m_depthStencilRenderbuffer{0};
    int m_renderWidth{0};
    int m_renderHeight{0};

    Core::CanvasNodeId m_dragNode{};
    glm::vec2 m_dragOffset{};
    Core::CanvasConnectorId m_pendingConnector{};
};
} // namespace ScopeCanvas::Studio

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Engine/DiagramModel.h"
#include "Interaction/CameraController.h"
#include "diagram/DiagramBasics.h"
#include "Interaction/ConnectController.h"
#include "Interaction/DragController.h"
#include "Interaction/EdgeInteractionController.h"
#include "Interaction/InputState.h"
#include "Interaction/SelectionController.h"
#include "Render/Renderer.h"
#include "View/Viewport.h"

#include <string>

class NodeDiagramWindow {
public:
    NodeDiagramWindow(
        GLFWwindow *window,
        Renderer *renderer,
        DiagramBasics *basics,
        GraphView *view,
        std::string windowTitle);

    ~NodeDiagramWindow();

    void Draw();

private:
    void handleShortcuts(bool focused);
    void ensureRenderTarget(int width, int height);
    void releaseRenderTarget();
    glm::vec2 screenToWorld(float localX, float localY) const;

    GLFWwindow *m_window;
    Renderer *m_renderer;
    DiagramBasics *m_basics;
    GraphView *m_view;
    std::string m_windowTitle;

    InputState m_input;
    CameraController m_cameraController;
    DragController m_dragController;
    SelectionController m_selectionController;
    ConnectController m_connectController;

    uint32_t m_hoveredEdgeId = 0;
    uint32_t m_hoveredConnectorId = 0;

    unsigned int m_framebuffer = 0;
    unsigned int m_colorTexture = 0;
    unsigned int m_depthStencilRenderbuffer = 0;
    int m_renderWidth = 0;
    int m_renderHeight = 0;
};

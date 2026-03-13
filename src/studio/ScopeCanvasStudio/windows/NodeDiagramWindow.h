#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Engine/DiagramModel.h"
#include "Interaction/CameraController.h"
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
        DiagramModel *graph,
        GraphView *view,
        std::string windowTitle);

    void Draw();

private:
    glm::vec2 screenToWorld(float localX, float localY) const;

    GLFWwindow *m_window;
    Renderer *m_renderer;
    DiagramModel *m_graph;
    GraphView *m_view;
    std::string m_windowTitle;

    InputState m_input;
    CameraController m_cameraController;
    DragController m_dragController;
    SelectionController m_selectionController;
    ConnectController m_connectController;

    uint32_t m_hoveredEdgeId = 0;
    uint32_t m_hoveredConnectorId = 0;
};

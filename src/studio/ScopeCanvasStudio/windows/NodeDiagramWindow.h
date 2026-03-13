#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Render/Renderer.h"
#include "Engine/DiagramModel.h"
#include "View/Camera2D.h"
#include "View/Viewport.h"

class NodeDiagramWindow {
public:
    NodeDiagramWindow(
        GLFWwindow *window,
        Renderer *renderer,
        DiagramModel *graph,
        Camera2D *camera);

    void Draw();

private:
    GLFWwindow *m_window;
    Renderer *m_renderer;
    DiagramModel *m_graph;
    Camera2D *m_camera;
};
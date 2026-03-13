#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "NodeDiagramWindow.h"

NodeDiagramWindow::NodeDiagramWindow(
    GLFWwindow *window,
    Renderer *renderer,
    DiagramModel *graph,
    Camera2D *camera) {
    m_window = window;
    m_renderer = renderer;
    m_graph = graph;
    m_camera = camera;
}

void NodeDiagramWindow::Draw() {
    ImGui::Begin("NodeDiagramWindow");

    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    ImGui::InvisibleButton("canvas", canvasSize);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);

    int vx = (int)canvasPos.x;
    int vy = (int)(fbHeight - canvasPos.y - canvasSize.y);
    int vw = (int)canvasSize.x;
    int vh = (int)canvasSize.y;

    GLint oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    glViewport(vx, vy, vw, vh);

    glEnable(GL_SCISSOR_TEST);
    glScissor(vx, vy, vw, vh);

    // ---- ENGINE RENDER ----
    Viewport vp;     // interaction state

    GraphView view;
    view.cameraPosition = m_camera->position();
    view.zoom = m_camera->zoom();
    view.focusNode = 0;

    m_renderer->render(
        *m_graph,
        view,
        vp,
        true,
        32.0f
    );
    // -----------------------

    glDisable(GL_SCISSOR_TEST);

    glViewport(
        oldViewport[0],
        oldViewport[1],
        oldViewport[2],
        oldViewport[3]
    );

    ImGui::End();
}
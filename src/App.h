#pragma once

struct GLFWwindow;

#include "Engine/DiagramModel.h"
#include "Interaction/CameraController.h"
#include "Interaction/DragController.h"
#include "Interaction/InputState.h"
#include "Interaction/SelectionController.h"
#include "Render/Renderer.h"

#include <glm/vec2.hpp>

class App {
public:
    App();
    ~App();

    bool init();
    void run();

    void onFramebufferSizeChanged(int width, int height);
    void onCursorPos(double xPos, double yPos);
    void onMouseButton(int button, int action, int mods);
    void onScroll(double xOffset, double yOffset);

private:
    void shutdown();
    void processInput(float deltaTime);
    glm::vec2 screenToWorld(double mouseX, double mouseY) const;

    GLFWwindow* m_window;
    Renderer m_renderer;
    DiagramModel m_model;
    InputState m_input;
    SelectionController m_selectionController;
    DragController m_dragController;
    CameraController m_cameraController;
    bool m_initialized;
};

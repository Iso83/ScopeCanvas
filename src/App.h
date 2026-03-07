#pragma once

struct GLFWwindow;

#include "Engine/DiagramModel.h"
#include "Interaction/CameraController.h"
#include "Interaction/ConnectController.h"
#include "Interaction/DragController.h"
#include "Interaction/EdgeInteractionController.h"
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
    struct DraggingEdgeEndpointState {
        bool active = false;
        uint32_t edgeId = 0;
        bool startEndpoint = false;
    };

    void clearEdgeSelection();
    void selectEdge(uint32_t edgeId);

    void shutdown();
    void processInput(float deltaTime);
    glm::vec2 screenToWorld(double mouseX, double mouseY);

    GLFWwindow* m_window;
    Renderer m_renderer;
    DiagramModel m_model;
    InputState m_input;
    SelectionController m_selectionController;
    DragController m_dragController;
    ConnectController m_connectController;
    CameraController m_cameraController;
    uint32_t m_hoveredEdgeId;
    uint32_t m_selectedEdgeId;
    uint32_t m_hoveredConnectorId;
    DraggingEdgeEndpointState m_draggingEdgeEndpoint;
    bool m_deleteHandled;
    bool m_createHandled;
    bool m_duplicateHandled;
    bool m_initialized;
};

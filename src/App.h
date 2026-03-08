#pragma once

struct GLFWwindow;

#include "Engine/CanvasEngine.h"
#include "Engine/Persistence/GraphSerializer.h"
#include "Interaction/CameraController.h"
#include "Interaction/Commands/GraphCommands.h"
#include "Interaction/ConnectController.h"
#include "Interaction/DragController.h"
#include "Interaction/EdgeInteractionController.h"
#include "Interaction/InputState.h"
#include "Interaction/SelectionController.h"
#include "Render/Renderer.h"

#include <glm/vec2.hpp>

#include <string>
#include <unordered_map>
#include <vector>

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

    struct ClipboardNode {
        uint32_t originalNodeId = 0;
        std::string nodeTypeId;
        std::string title;
        glm::vec2 relativePosition = glm::vec2(0.0f);
        glm::vec2 size = glm::vec2(0.0f);
    };

    struct ClipboardEdge {
        uint32_t fromNode = 0;
        uint32_t fromConnector = 0;
        uint32_t toNode = 0;
        uint32_t toConnector = 0;
    };

    struct ClipboardData {
        glm::vec2 origin = glm::vec2(0.0f);
        std::vector<ClipboardNode> nodes;
        std::vector<ClipboardEdge> edges;
        bool empty() const { return nodes.empty(); }
    };

    void clearEdgeSelection();
    void selectEdge(uint32_t edgeId);

    void copySelectionToClipboard();
    void pasteClipboard();

    bool saveGraph();
    bool loadGraph();

    void shutdown();
    void processInput(float deltaTime);
    glm::vec2 screenToWorld(double mouseX, double mouseY);

    GLFWwindow* m_window;
    Renderer m_renderer;
    CanvasEngine m_engine;
    InputState m_input;
    SelectionController m_selectionController;
    DragController m_dragController;
    ConnectController m_connectController;
    CameraController m_cameraController;
    ClipboardData m_clipboard;
    uint32_t m_hoveredEdgeId;
    uint32_t m_selectedEdgeId;
    uint32_t m_hoveredConnectorId;
    DraggingEdgeEndpointState m_draggingEdgeEndpoint;
    bool m_deleteHandled;
    bool m_createHandled;
    bool m_duplicateHandled;
    bool m_copyHandled;
    bool m_pasteHandled;
    bool m_undoHandled;
    bool m_redoHandled;
    bool m_saveHandled;
    bool m_loadHandled;
    bool m_initialized;

};

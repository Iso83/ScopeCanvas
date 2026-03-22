#pragma once

#include <ScopeCanvas/core/GraphDocument.h>
#include <ScopeCanvas/render/CanvasRenderer.h>
#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/routing/EdgeRouter.h>
#include <glm/vec2.hpp>
#include <vector>

struct GLFWwindow;

class App {
  private:
    GLFWwindow* m_window{};
    bool m_initialized{false};

    struct InputState {
        double mouseX = 0.0;
        double mouseY = 0.0;
        bool leftDown = false;
        bool middleDown = false;
        bool previousLeftDown = false;
        bool previousMiddleDown = false;
        float scrollDelta = 0.0f;
    } m_input;

    ScopeCanvas::Core::GraphDocument m_document{};
    ScopeCanvas::Render::CanvasRenderer m_renderer{};
    ScopeCanvas::Render::Camera::Camera2D m_camera{};
    ScopeCanvas::Render::CanvasRenderOptions m_renderOptions{};
    ScopeCanvas::Routing::EdgeRouter m_router{};
    std::vector<ScopeCanvas::Core::CanvasNodeId> m_nodeIds{};

    ScopeCanvas::Core::CanvasNodeId m_dragNode{};
    ScopeCanvas::Core::CanvasNodeId m_selectedNode{};
    glm::vec2 m_dragOffset{};
    bool m_selectionRectActive{false};
    glm::vec2 m_selectionRectStart{};
    glm::vec2 m_selectionRectEnd{};

  public:
    App();
    ~App();

    bool init();
    void run();

    void onFramebufferSizeChanged(int width, int height);
    void onCursorPos(double xPos, double yPos) {
        m_input.mouseX = xPos;
        m_input.mouseY = yPos;
    }
    void onMouseButton(int button, int action, int mods);
    void onScroll(double xOffset, double yOffset) {
        (void)xOffset;
        m_input.scrollDelta += static_cast<float>(yOffset);
    }

  private:
    bool initWindow();
    bool initGL();
    void initializeDocument();
    void shutdown();
    void processInput(float deltaTime);
    void setupCallbacks();
    glm::vec2 screenToWorld(double x, double y) const;
};

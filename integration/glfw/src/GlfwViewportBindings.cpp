#include <ScopeCanvas/glfw/GlfwViewportBindings.h>
#include <ScopeCanvas/render/window/Viewport.h>
#include <unordered_map>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ScopeCanvas::GLFW {

using namespace ScopeCanvas::Render::Window;

inline std::unordered_map<GLFWwindow*, Render::Window::ViewportHandler*> s_viewportHandlers;

ViewportHandler* viewportHandler(GLFWwindow* window) {
    auto it = s_viewportHandlers.find(window);
    return it != s_viewportHandlers.end() ? it->second : nullptr;
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    viewportHandler(window)->processMouseMove({x, y});
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_REPEAT)
        return;

    bool pressed{action == GLFW_PRESS};

    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        viewportHandler(window)->processMouseButton(SC_MOUSE_BUTTON_LEFT, pressed);
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        viewportHandler(window)->processMouseButton(SC_MOUSE_BUTTON_MIDDLE, pressed);
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        viewportHandler(window)->processMouseButton(SC_MOUSE_BUTTON_RIGHT, pressed);
        break;
    }
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    viewportHandler(window)->processScroll(xOffset, yOffset);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_REPEAT)
        return;

    viewportHandler(window)->processKey(key, action == GLFW_PRESS);
}

void bindViewportHandler(GLFWwindow* window, ViewportHandler* handler) {
    s_viewportHandlers[window] = handler;

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);
}

void updateViewportSize(GLFWwindow *window, Viewport *view) {
    int width = 1;
    int height = 1;
    glfwGetFramebufferSize(window, &width, &height);
    view->setViewportSize(width, height);
}
} // namespace ScopeCanvas::GLFW
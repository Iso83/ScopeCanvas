#include <ScopeCanvas/glfw/GlfwInputListener.h>
#include <ScopeCanvas/glfw/GlfwViewportBindings.h>
#include <ScopeCanvas/render/window/Viewport.h>
#include <unordered_map>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <ScopeCanvas/render/gl/OpenGLApi.h>

namespace ScopeCanvas::GLFW {

using namespace ScopeCanvas::Render::Window;

ScopeCanvas::Input::Key mapGlfwKey(int key) {
    using ScopeCanvas::Input::Key;
    switch (key) {
    case GLFW_KEY_ESCAPE: return Key::Escape;
    case GLFW_KEY_DELETE: return Key::Delete;
    case GLFW_KEY_1: return Key::Digit1;
    case GLFW_KEY_2: return Key::Digit2;
    case GLFW_KEY_3: return Key::Digit3;
    case GLFW_KEY_4: return Key::Digit4;
    case GLFW_KEY_A: return Key::A;
    case GLFW_KEY_D: return Key::D;
    case GLFW_KEY_W: return Key::W;
    case GLFW_KEY_S: return Key::S;
    case GLFW_KEY_G: return Key::G;
    case GLFW_KEY_LEFT: return Key::Left;
    case GLFW_KEY_RIGHT: return Key::Right;
    case GLFW_KEY_UP: return Key::Up;
    case GLFW_KEY_DOWN: return Key::Down;
    default: return Key::Unknown;
    }
}

std::unordered_map<GLFWwindow*, Render::Window::ViewportHandler*> s_viewportHandlers;
std::unordered_map<GLFWwindow*, GlfwInputListener*> s_inputListeners;
GlfwInputListener* s_defaultInputListener{};

ViewportHandler* viewportHandler(GLFWwindow* window) {
    auto it = s_viewportHandlers.find(window);
    return it != s_viewportHandlers.end() ? it->second : nullptr;
}

GlfwInputListener* inputListener(GLFWwindow* window) {
    auto it = s_inputListeners.find(window);
    return it != s_inputListeners.end() ? it->second : s_defaultInputListener;
}

void bindInputListener(GLFWwindow* window, GlfwInputListener* listener) {
    if (listener)
        s_inputListeners[window] = listener;
    else
        s_inputListeners.erase(window);
}

void setInputListener(GlfwInputListener* listener) {
    s_defaultInputListener = listener;
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    if (auto* listener = inputListener(window))
        listener->cursorPosCallback(window, x, y);

    if (auto* handler = viewportHandler(window))
        handler->processMouseMove({x, y});
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (auto* listener = inputListener(window))
        listener->mouseButtonCallback(window, button, action, mods);

    if (action == GLFW_REPEAT)
        return;

    auto* handler = viewportHandler(window);
    if (!handler)
        return;

    bool pressed{action == GLFW_PRESS};

    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        handler->processMouseButton(SC_MOUSE_BUTTON_LEFT, pressed);
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        handler->processMouseButton(SC_MOUSE_BUTTON_MIDDLE, pressed);
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        handler->processMouseButton(SC_MOUSE_BUTTON_RIGHT, pressed);
        break;
    }
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    if (auto* listener = inputListener(window))
        listener->scrollCallback(window, xOffset, yOffset);

    if (auto* handler = viewportHandler(window))
        handler->processScroll(xOffset, yOffset);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (auto* listener = inputListener(window))
        listener->keyCallback(window, key, scancode, action, mods);

    if (action == GLFW_REPEAT)
        return;

    if (auto* handler = viewportHandler(window))
        handler->processKey(mapGlfwKey(key), action == GLFW_PRESS);
}

void installCallbacks(GLFWwindow* window) {
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);
}

void bindViewportHandler(GLFWwindow* window, ViewportHandler* handler) {
    if (handler)
        s_viewportHandlers[window] = handler;
    else
        s_viewportHandlers.erase(window);

    installCallbacks(window);
}

void updateViewportSize(GLFWwindow* window, Viewport* view) {
    int width = 1;
    int height = 1;
    glfwGetFramebufferSize(window, &width, &height);
    view->setViewportSize(width, height);
}

void prepareFramebuffer(GLFWwindow* window, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    int width = 1;
    int height = 1;
    glfwGetFramebufferSize(window, &width, &height);
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}
} // namespace ScopeCanvas::GLFW

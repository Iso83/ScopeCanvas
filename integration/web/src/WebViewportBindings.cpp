#include <ScopeCanvas/web/WebViewportBindings.h>
#include <ScopeCanvas/input/InputCodes.h>
#include <ScopeCanvas/render/gl/OpenGLApi.h>
#include <ScopeCanvas/render/window/Viewport.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>
#include <algorithm>
#include <unordered_map>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ScopeCanvas::Web {
namespace {
std::unordered_map<GLFWwindow*, Render::Window::ViewportHandler*> s_handlers;

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

Render::Window::ViewportHandler* handlerFor(GLFWwindow* window) {
    const auto it = s_handlers.find(window);
    return it == s_handlers.end() ? nullptr : it->second;
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    if (auto* handler = handlerFor(window))
        handler->processMouseMove({x, y});
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/) {
    if (action == GLFW_REPEAT)
        return;
    auto* handler = handlerFor(window);
    if (handler == nullptr)
        return;
    const bool pressed = action == GLFW_PRESS;
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT: handler->processMouseButton(Render::Window::SC_MOUSE_BUTTON_LEFT, pressed); break;
    case GLFW_MOUSE_BUTTON_MIDDLE: handler->processMouseButton(Render::Window::SC_MOUSE_BUTTON_MIDDLE, pressed); break;
    case GLFW_MOUSE_BUTTON_RIGHT: handler->processMouseButton(Render::Window::SC_MOUSE_BUTTON_RIGHT, pressed); break;
    default: break;
    }
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    if (auto* handler = handlerFor(window))
        handler->processScroll(xOffset, yOffset);
}

void keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (action == GLFW_REPEAT)
        return;
    if (auto* handler = handlerFor(window))
        handler->processKey(mapGlfwKey(key), action == GLFW_PRESS);
}
} // namespace

bool initializeWebGlfw() {
    return glfwInit() == GLFW_TRUE;
}

GLFWwindow* createWebGL2Canvas(int width, int height, const char* title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(std::max(width, 1), std::max(height, 1), title, nullptr, nullptr);
    if (window != nullptr)
        glfwMakeContextCurrent(window);
    return window;
}

void bindViewportHandler(GLFWwindow* window, ScopeCanvas::Render::Window::ViewportHandler* handler) {
    if (handler != nullptr)
        s_handlers[window] = handler;
    else
        s_handlers.erase(window);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);
}

void updateViewportSize(GLFWwindow* window, ScopeCanvas::Render::Window::Viewport* view) {
    int width = 1;
    int height = 1;
    glfwGetFramebufferSize(window, &width, &height);
    view->setViewportSize(std::max(width, 1), std::max(height, 1));
}

void prepareFrame(float red, float green, float blue, float alpha) {
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
} // namespace ScopeCanvas::Web

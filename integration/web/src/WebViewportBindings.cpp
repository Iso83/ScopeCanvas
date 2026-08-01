#include <ScopeCanvas/engine/render/gl/OpenGLApi.h>
#include <ScopeCanvas/engine/render/window/Viewport.h>
#include <ScopeCanvas/integration/web/WebViewportBindings.h>
#include <algorithm>
#include <cmath>
#include <emscripten/html5.h>
#include <unordered_map>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace ScopeCanvas::Engine::Render::Window;

namespace ScopeCanvas::Integration::Web {

constexpr const char* CanvasSelector = "#canvas";

std::unordered_map<GLFWwindow*, ViewportHandler*> s_viewportHandlers;

ViewportHandler* viewportHandler(GLFWwindow* window) {
    const auto it = s_viewportHandlers.find(window);
    return it == s_viewportHandlers.end() ? nullptr : it->second;
}

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

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    if (auto* handler = viewportHandler(window))
        handler->processMouseMove({x, y});
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_REPEAT)
        return;

    auto* handler = viewportHandler(window);
    if (handler == nullptr)
        return;

    const bool pressed = action == GLFW_PRESS;

    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            handler->processMouseButton(Engine::Render::Window::SC_MOUSE_BUTTON_LEFT, pressed);
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            handler->processMouseButton(Engine::Render::Window::SC_MOUSE_BUTTON_MIDDLE, pressed);
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            handler->processMouseButton(Engine::Render::Window::SC_MOUSE_BUTTON_RIGHT, pressed);
            break;
        default:
            break;
    }
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    if (auto* handler = viewportHandler(window))
        handler->processScroll(xOffset, yOffset);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_REPEAT)
        return;

    if (auto* handler = viewportHandler(window))
        handler->processKey(key, action == GLFW_PRESS);
}

void installCallbacks(GLFWwindow* window) {
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);
}

void bindViewportHandler(GLFWwindow* window, ViewportHandler* handler) {
    if (handler != nullptr)
        s_viewportHandlers[window] = handler;
    else
        s_viewportHandlers.erase(window);

    installCallbacks(window);
}

void updateViewportSize(GLFWwindow* window, Viewport* view) {
    double cssWidth = 0.0;
    double cssHeight = 0.0;
    if (emscripten_get_element_css_size(CanvasSelector, &cssWidth, &cssHeight) == EMSCRIPTEN_RESULT_SUCCESS) {
        const int displayWidth = std::max(1, static_cast<int>(std::lround(cssWidth)));
        const int displayHeight = std::max(1, static_cast<int>(std::lround(cssHeight)));

        int canvasWidth = 0;
        int canvasHeight = 0;
        emscripten_get_canvas_element_size(CanvasSelector, &canvasWidth, &canvasHeight);
        if (canvasWidth != displayWidth || canvasHeight != displayHeight) {
            emscripten_set_canvas_element_size(CanvasSelector, displayWidth, displayHeight);
            glfwSetWindowSize(window, displayWidth, displayHeight);
        }
    }

    int width = 1;
    int height = 1;
    glfwGetFramebufferSize(window, &width, &height);
    view->setViewportSize(std::max(width, 1), std::max(height, 1));
}

void prepareFramebuffer(float red, float green, float blue, float alpha) {
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
} // namespace ScopeCanvas::Integration::Web

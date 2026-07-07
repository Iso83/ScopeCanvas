#pragma once

struct GLFWwindow;

namespace ScopeCanvas::GLFW {
/**
 * Optional listener that receives GLFW input events intercepted by
 * bindViewportHandler().
 *
 * This can be used by external systems (e.g. ImGui) that also require
 * access to GLFW input events after ScopeCanvas has installed its own
 * callbacks.
 */
class GlfwInputListener {
  public:
    virtual ~GlfwInputListener() = default;

    virtual void cursorPosCallback(GLFWwindow* window, double x, double y) {}
    virtual void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {}
    virtual void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {}
    virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {}
};

void bindInputListener(GLFWwindow* window, GlfwInputListener* sink);
void setInputListener(GlfwInputListener* sink);
} // namespace ScopeCanvas::GLFW
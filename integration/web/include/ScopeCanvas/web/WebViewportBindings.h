#pragma once

struct GLFWwindow;

namespace ScopeCanvas::Render::Window {
class Viewport;
class ViewportHandler;
}

namespace ScopeCanvas::Web {
bool initializeWebGlfw();
GLFWwindow* createWebGL2Canvas(int width, int height, const char* title);
void bindViewportHandler(GLFWwindow* window, ScopeCanvas::Render::Window::ViewportHandler* handler);
void updateViewportSize(GLFWwindow* window, ScopeCanvas::Render::Window::Viewport* view);
void prepareFrame(float red, float green, float blue, float alpha);
} // namespace ScopeCanvas::Web

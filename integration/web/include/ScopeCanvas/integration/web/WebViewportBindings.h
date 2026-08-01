#pragma once

struct GLFWwindow;

namespace ScopeCanvas::Engine::Render::Window {

class Viewport;
class ViewportHandler;

} // namespace ScopeCanvas::Engine::Render::Window

namespace ScopeCanvas::Integration::Web {

bool initializeWebGlfw();
GLFWwindow* createWebGL2Canvas(int width, int height, const char* title);

void bindViewportHandler(GLFWwindow* window, Engine::Render::Window::ViewportHandler* handler);
void updateViewportSize(GLFWwindow* window, Engine::Render::Window::Viewport* view);
void prepareFramebuffer(float red, float green, float blue, float alpha);

} // namespace ScopeCanvas::Integration::Web

#pragma once

#include <ScopeCanvas/render/window/ViewportHandler.h>

class GLFWwindow;

namespace ScopeCanvas::GLFW {
void bindViewportHandler(GLFWwindow* window, Render::Window::ViewportHandler* handler);
void updateViewportSize(GLFWwindow* window, Render::Window::Viewport* view);
} // namespace ScopeCanvas::GLFW
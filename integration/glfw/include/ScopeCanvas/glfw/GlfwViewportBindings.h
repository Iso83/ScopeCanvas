#pragma once

#include <ScopeCanvas/render/window/ViewportHandler.h>
#include <glad/glad.h>

struct GLFWwindow;

namespace ScopeCanvas::GLFW {
void installCallbacks(GLFWwindow* window);
void bindViewportHandler(GLFWwindow* window, Render::Window::ViewportHandler* handler);
void updateViewportSize(GLFWwindow* window, Render::Window::Viewport* view);
void prepareFramebuffer(GLFWwindow* window, GLfloat red = 0.1F, GLfloat green = 0.1F, GLfloat blue = 0.1F,
                        GLfloat alpha = 1.0F);
} // namespace ScopeCanvas::GLFW

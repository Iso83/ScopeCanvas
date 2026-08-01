#pragma once

#include <ScopeCanvas/engine/render/gl/OpenGLApi.h>
#include <ScopeCanvas/engine/render/window/ViewportHandler.h>

struct GLFWwindow;

namespace ScopeCanvas::Integration::GLFW {
void installCallbacks(GLFWwindow* window);
void bindViewportHandler(GLFWwindow* window, Engine::Render::Window::ViewportHandler* handler);
void updateViewportSize(GLFWwindow* window, Engine::Render::Window::Viewport* view);
void prepareFramebuffer(GLFWwindow* window, GLfloat red = 0.1F, GLfloat green = 0.1F, GLfloat blue = 0.1F,
                        GLfloat alpha = 1.0F);
} // namespace ScopeCanvas::Integration::GLFW

#pragma once

struct GLFWwindow;

namespace ScopeCanvas::Integration::GLFW {
bool initializeGLFW();
bool initializeGLAD();

GLFWwindow* createOpenGLWindow(int width, int height, const char* title, bool bootstrapOpenGL = false,
                               int swapInterval = 1);
} // namespace ScopeCanvas::Integration::GLFW

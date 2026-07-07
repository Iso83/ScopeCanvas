#pragma once

struct GLFWwindow;

namespace ScopeCanvas::GLFW {
bool initializeGLFW();
bool initializeGLAD();

GLFWwindow* createOpenGLWindow(int width, int height, const char* title, bool bootstrapOpenGL = false, int swapInterval = 1);
} // namespace ScopeCanvas::GLFW
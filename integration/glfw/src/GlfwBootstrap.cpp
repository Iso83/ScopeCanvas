#include <glad/glad.h>
#include <iostream>
#include <ScopeCanvas/glfw/GlfwBootstrap.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ScopeCanvas::GLFW {

bool initializeGLFW() {
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return true;
}

bool initializeGLAD() {
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize OpenGL loader\n";
        return false;
    }

    return true;
}

GLFWwindow* createOpenGLWindow(int width, int height, const char* title, bool bootstrapOpenGL, int swapInterval) {
    if (bootstrapOpenGL && !initializeGLFW())
        return nullptr;

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        if (bootstrapOpenGL)
            glfwTerminate();

        return nullptr;
    }
    glfwMakeContextCurrent(window);

    glfwSwapInterval(swapInterval);

    if (bootstrapOpenGL && !initializeGLAD()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    return window;
}

} // namespace ScopeCanvas::GLFW
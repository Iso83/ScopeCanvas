#include "DemoViewport.h"

#include <ScopeCanvas/demo/DiagramDrawContext.h>
#include <ScopeCanvas/glfw/GlfwBootstrap.h>
#include <ScopeCanvas/glfw/GlfwViewportBindings.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace ScopeCanvas::Render::Window;
using namespace ScopeCanvas::Demo;
using namespace ScopeCanvas::GLFW;

int main() {
    GLFWwindow* window = createOpenGLWindow(1280, 720, "ScopeCanvas OpenGL Demo", true);
    if (window == nullptr)
        return -1;

    DiagramDrawCtx drawCtx{};

    DemoViewport view{};
    view.registerDrawContext(&drawCtx);
    view.setViewPosition({120.0F, 0.0F});

    ViewportHandler viewHandler;
    viewHandler.registerViewport(&view);

    bindViewportHandler(window, &viewHandler);

    while (!glfwWindowShouldClose(window)) {
        if (!view.needsRender())
            glfwWaitEvents();
        else
            glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        updateViewportSize(window, &view);

        view.draw();

        viewHandler.updatePrevInteraction();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
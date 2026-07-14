#include "FlowDrawContext.h"
#include "FlowViewport.h"
#include <ScopeCanvas/glfw/GlfwBootstrap.h>
#include <ScopeCanvas/glfw/GlfwViewportBindings.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace ScopeCanvas::GLFW;
using namespace ScopeCanvas::Render::Window;

int main() {
    GLFWwindow* window = createOpenGLWindow(1280, 720, "ScopeCanvas Flow Demo", true);
    if (window == nullptr)
        return -1;

    {
        FlowDrawContext drawCtx{};
        FlowViewport view{};
        view.registerDrawContext(&drawCtx);
        view.setViewPosition({360.0F, 0.0F});
        view.setViewZoom(1.15F);

        ViewportHandler viewHandler;
        viewHandler.registerViewport(&view);
        bindViewportHandler(window, &viewHandler);

        while (!glfwWindowShouldClose(window)) {
            viewHandler.needsRender() ? glfwPollEvents() : glfwWaitEvents();
            updateViewportSize(window, &view);
            viewHandler.draw();
            viewHandler.updatePrevInteraction();
            glfwSwapBuffers(window);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

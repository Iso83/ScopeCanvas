#include "FlowDrawContext.h"
#include "FlowViewport.h"

#include <ScopeCanvas/integration/glfw/GlfwBootstrap.h>
#include <ScopeCanvas/integration/glfw/GlfwViewportBindings.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace ScopeCanvas::Demo::Flow;
using namespace ScopeCanvas::Engine::Render::Window;
using namespace ScopeCanvas::Integration::GLFW;

int runDemo(GLFWwindow* window) {
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

    return 0;
}

int main() {
    GLFWwindow* window = createOpenGLWindow(1280, 720, "ScopeCanvas Flow Demo", true);
    if (window == nullptr)
        return -1;

    int result = runDemo(window);

    glfwDestroyWindow(window);
    glfwTerminate();

    return result;
}

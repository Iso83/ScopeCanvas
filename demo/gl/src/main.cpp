#include "DemoViewport.h"

#include <ScopeCanvas/demo/DiagramDrawContext.h>
#include <ScopeCanvas/glfw/GlfwBootstrap.h>
#include <ScopeCanvas/glfw/GlfwViewportBindings.h>
#include <ScopeCanvas/render/RenderBenchmark.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace ScopeCanvas::Render;
using namespace ScopeCanvas::Render::Window;
using namespace ScopeCanvas::Demo;
using namespace ScopeCanvas::GLFW;

#ifdef SC_BUILD_DEMO_BENCHMARK
#include <iostream>

static void printDebugInfo(const RenderBenchmark& benchmark) {
    static double lastPrint = glfwGetTime();

    const double now = glfwGetTime();
    if (now - lastPrint < 1.0)
        return;

    lastPrint = now;

    const auto& stats = benchmark.statistics();

    std::cout << std::fixed << std::setprecision(2) << "[ScopeCanvas] "
              << "FPS: " << stats.framesPerSecond << " | Est: " << stats.estimatedFramesPerSecond
              << " | Frame: " << stats.averageFrameTimeMs << " ms"
              << " | Load: " << stats.renderLoadPercent << '%' << " | Idle: " << stats.idleTimeMs << " ms"
              << " | Frames: " << stats.renderedFrames << " | Time: " << stats.elapsedSeconds << " s" << '\n';
}
#endif

int main() {
    GLFWwindow* window = createOpenGLWindow(1280, 720, "ScopeCanvas OpenGL Demo", true, SC_SWAPINTERVAL);
    if (window == nullptr)
        return -1;

    DiagramDrawCtx drawCtx{};

    DemoViewport view{};
    view.registerDrawContext(&drawCtx);
    view.setViewPosition({120.0F, 0.0F});

    ViewportHandler viewHandler;
    viewHandler.registerViewport(&view);

    bindViewportHandler(window, &viewHandler);

#ifdef SC_BUILD_DEMO_BENCHMARK
    RenderBenchmark benchmark{};
#endif

    while (!glfwWindowShouldClose(window)) {
        viewHandler.needsRender() ? glfwPollEvents() : glfwWaitEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        updateViewportSize(window, &view);

#ifdef SC_BUILD_DEMO_BENCHMARK
        viewHandler.draw(&benchmark);
        if (benchmark.updated())
            printDebugInfo(benchmark);
#else
        viewHandler.draw();
#endif

        viewHandler.updatePrevInteraction();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
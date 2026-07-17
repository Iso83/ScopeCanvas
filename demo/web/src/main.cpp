#include "../../gl/src/DemoViewport.h"

#include <ScopeCanvas/demo/DiagramDrawContext.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>
#include <ScopeCanvas/web/WebViewportBindings.h>
#include <emscripten.h>
#include <memory>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace {
struct WebDemoApp {
    GLFWwindow* window{};
    ScopeCanvas::Demo::DiagramDrawCtx drawCtx{};
    DemoViewport view{};
    ScopeCanvas::Render::Window::ViewportHandler handler{};
};

void frame(void* userData) {
    auto& app = *static_cast<WebDemoApp*>(userData);
    if (app.window == nullptr)
        return;

    ScopeCanvas::Web::updateViewportSize(app.window, &app.view);
    if (app.handler.needsRender()) {
        ScopeCanvas::Web::prepareFrame(0.08F, 0.09F, 0.11F, 1.0F);
        app.handler.draw();
        glfwSwapBuffers(app.window);
    }
    app.handler.updatePrevInteraction();
    glfwPollEvents();
}
} // namespace

int main() {
    if (!ScopeCanvas::Web::initializeWebGlfw())
        return 1;

    static auto app = std::make_unique<WebDemoApp>();
    app->window = ScopeCanvas::Web::createWebGL2Canvas(1280, 720, "ScopeCanvas WebGL2 Demo");
    if (app->window == nullptr)
        return 2;

    app->view.registerDrawContext(&app->drawCtx);
    app->view.setViewPosition({120.0F, 0.0F});
    app->handler.registerViewport(&app->view);
    ScopeCanvas::Web::bindViewportHandler(app->window, &app->handler);

    emscripten_set_main_loop_arg(frame, app.get(), 0, true);
    return 0;
}

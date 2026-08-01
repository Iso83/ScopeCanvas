#include <ScopeCanvas/demo/web/Web.h>
#include <ScopeCanvas/integration/web/WebViewportBindings.h>
#include <emscripten.h>
#include <memory>

using namespace ScopeCanvas::Integration::Web;
using namespace ScopeCanvas::Demo::Web;

void frame(void* userData) {
    auto& app = *static_cast<WebDemoApp*>(userData);
    if (app.window == nullptr)
        return;

    updateViewportSize(app.window, &app.view);
    if (app.handler.needsRender()) {
        prepareFramebuffer(0.08F, 0.09F, 0.11F, 1.0F);
        app.handler.draw();
        glfwSwapBuffers(app.window);
    }
    app.handler.updatePrevInteraction();

    glfwPollEvents();
}

int main() {
    if (!initializeWebGlfw())
        return 1;

    static auto app = std::make_unique<WebDemoApp>();
    app->window = createWebGL2Canvas(1280, 720, "ScopeCanvas WebGL2 Demo");
    if (app->window == nullptr)
        return 2;

    app->view.registerDrawContext(&app->drawCtx);
    app->view.setViewPosition({120.0F, 0.0F});
    app->handler.registerViewport(&app->view);
    bindViewportHandler(app->window, &app->handler);

    emscripten_set_main_loop_arg(frame, app.get(), 0, true);
    return 0;
}

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <ScopeCanvas/demo/common/DemoViewport.h>
#include <ScopeCanvas/demo/common/DiagramDrawContext.h>
#include <ScopeCanvas/engine/render/window/ViewportHandler.h>

namespace ScopeCanvas::Demo::Web {
struct WebDemoApp {
    GLFWwindow* window{};
    Common::DiagramDrawCtx drawCtx{};
    Common::DemoViewport view{};
    Engine::Render::Window::ViewportHandler handler{};
};
} // namespace ScopeCanvas::Demo::Web

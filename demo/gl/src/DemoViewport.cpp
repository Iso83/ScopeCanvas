#include "DemoViewport.h"
#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/demo/DiagramDrawContext.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace ScopeCanvas::Demo;
using namespace ScopeCanvas::Core::Ids;

bool DemoViewport::processOnDraw_KeyStroke() {
    auto handler = this->handler();
    const auto deltaTime = this->deltaTime();

    if (handler) {
        constexpr float panSpeed = 500.0F;
        glm::vec2 panDelta{0.0F, 0.0F};
        if (handler->keyState(ScopeCanvas::Input::Key::A).down)
            panDelta.x -= panSpeed * deltaTime;
        if (handler->keyState(ScopeCanvas::Input::Key::D).down)
            panDelta.x += panSpeed * deltaTime;
        if (handler->keyState(ScopeCanvas::Input::Key::W).down)
            panDelta.y += panSpeed * deltaTime;
        if (handler->keyState(ScopeCanvas::Input::Key::S).down)
            panDelta.y -= panSpeed * deltaTime;

        if (panDelta.x || panDelta.y) {
            moveView(panDelta);
            return true;
        }
    }

    return false;
}

void DemoViewport::onKey(ScopeCanvas::Input::Key key, bool pressed) {
    switch (key) {
    case ScopeCanvas::Input::Key::A:
    case ScopeCanvas::Input::Key::D:
    case ScopeCanvas::Input::Key::W:
    case ScopeCanvas::Input::Key::S:
    case ScopeCanvas::Input::Key::G:
        m_needsRender = true;
        break;
    }

    // process interaction on ctx
    if (!pressed || !handler() || m_activeState == InvalidState)
        return;

    auto* ctx = static_cast<DiagramDrawCtx*>(activeState().drawContext);
    if (ctx == nullptr)
        return;

    switch (key) {
    case ScopeCanvas::Input::Key::G:
        ctx->showGrid() = !ctx->showGrid();
        break;

    case ScopeCanvas::Input::Key::Digit1:
        ctx->createNodeAtCenter(camera(), NodeTypeId{1});
        break;

    case ScopeCanvas::Input::Key::Digit2:
        ctx->createNodeAtCenter(camera(), NodeTypeId{2});
        break;

    case ScopeCanvas::Input::Key::Digit3:
        ctx->createNodeAtCenter(camera(), NodeTypeId{3});
        break;

    case ScopeCanvas::Input::Key::Digit4:
        ctx->createNodeAtCenter(camera(), NodeTypeId{4});
        break;

    default:
        return;
    }

    m_needsRender = true;
}

void DemoViewport::draw(ScopeCanvas::Render::Window::DrawContext* ctx) {
    m_nowTime = glfwGetTime();

    bool keyStroked = processOnDraw_KeyStroke();
    Viewport::draw(ctx);

    m_needsRender = keyStroked || ctx->needsRender();
    m_lastTime = m_nowTime;
}
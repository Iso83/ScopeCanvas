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
        if (handler->keyState(GLFW_KEY_A).down)
            panDelta.x -= panSpeed * deltaTime;
        if (handler->keyState(GLFW_KEY_D).down)
            panDelta.x += panSpeed * deltaTime;
        if (handler->keyState(GLFW_KEY_W).down)
            panDelta.y += panSpeed * deltaTime;
        if (handler->keyState(GLFW_KEY_S).down)
            panDelta.y -= panSpeed * deltaTime;

        if (panDelta.x || panDelta.y) {
            moveView(panDelta);
            return true;
        }
    }

    return false;
}

void DemoViewport::onKey(int key, bool pressed) {
    switch (key) {
    case GLFW_KEY_A:
    case GLFW_KEY_D:
    case GLFW_KEY_W:
    case GLFW_KEY_S:
    case GLFW_KEY_G:
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
    case GLFW_KEY_G:
        ctx->showGrid() = !ctx->showGrid();
        break;

    case GLFW_KEY_1:
        ctx->createNodeAtCenter(camera(), NodeTypeId{1});
        break;

    case GLFW_KEY_2:
        ctx->createNodeAtCenter(camera(), NodeTypeId{2});
        break;

    case GLFW_KEY_3:
        ctx->createNodeAtCenter(camera(), NodeTypeId{3});
        break;

    case GLFW_KEY_4:
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
    __super::draw(ctx);

    m_needsRender = keyStroked || ctx->needsRender();
    m_lastTime = m_nowTime;
}
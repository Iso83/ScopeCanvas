#include "FlowViewport.h"
#include "FlowDrawContext.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void FlowViewport::onKey(int key, bool pressed) {
    if (!pressed || m_activeState == InvalidState)
        return;

    switch (key) {
    case GLFW_KEY_A:
    case GLFW_KEY_LEFT:
        moveView({-180.0F, 0.0F});
        break;
    case GLFW_KEY_D:
    case GLFW_KEY_RIGHT:
        moveView({180.0F, 0.0F});
        break;
    case GLFW_KEY_W:
    case GLFW_KEY_UP:
        moveView({0.0F, 140.0F});
        break;
    case GLFW_KEY_S:
    case GLFW_KEY_DOWN:
        moveView({0.0F, -140.0F});
        break;
    default:
        return;
    }

    const auto flowContext = [](auto& state) { return static_cast<FlowDrawContext*>(state.drawContext); };

    if (auto* ctx = flowContext(activeState()); ctx != nullptr)
        ctx->clampViewToContent(this);
    m_needsRender = true;
}

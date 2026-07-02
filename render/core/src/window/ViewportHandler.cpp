#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/render/window/Viewport.h>
#include <ScopeCanvas/render/window/viewporthandler.h>

namespace ScopeCanvas::Render::Window {
//-------------------------------------------------------------------------
// Viewports
//-------------------------------------------------------------------------
void ViewportHandler::registerViewport(Viewport* viewport) {
    if (viewport == nullptr)
        return;

    if (m_viewports.empty())
        m_activeView = 0;

    if (std::find(m_viewports.begin(), m_viewports.end(), viewport) == m_viewports.end())
        m_viewports.push_back(viewport);

    viewport->m_handler = this;
}

void ViewportHandler::unregisterViewport(Viewport* viewport) {
    if (viewport == nullptr)
        return;

    viewport->m_handler = nullptr;

    if (activeViewport() == viewport)
        m_activeView = InvalidView;

    const auto it = std::remove(m_viewports.begin(), m_viewports.end(), viewport);
    m_viewports.erase(it, m_viewports.end());

    if (m_viewports.empty())
        m_activeView = InvalidView;
}

//-------------------------------------------------------------------------
// Interaction
//-------------------------------------------------------------------------
bool ViewportHandler::processMouseMove(const glm::vec2 pos) {
    if (m_mousePosition == pos)
        return false;

    m_mousePosition = pos;

    forEachInteractionRecipient([&pos](auto* target) {
        if (target != nullptr && target->handlesMouseMove())
            target->onMouseMove(pos);
    });

    return true;
}

void ViewportHandler::processMouseButton(MouseButton button, bool pressed) {
    if (mouseState(button).down == pressed)
        return;

    m_mouseButtons[button].down = pressed;

    forEachInteractionRecipient([&](auto* target) {
        if (target != nullptr && target->handlesMouseButton())
            target->onMouseButton(button, pressed);
    });
}

void ViewportHandler::processScroll(double xOffset, double yOffset) {
    forEachInteractionRecipient([&](auto* target) {
        if (target != nullptr && target->handlesScroll())
            target->onScroll(xOffset, yOffset);
    });
}

void ViewportHandler::processKey(int key, bool pressed) {
    if (key < 0 || keyState(key).down == pressed)
        return;

    m_keys[key].down = pressed;

    forEachInteractionRecipient([&](auto* target) {
        if (target != nullptr && target->handlesKey())
            target->onKey(key, pressed);
    });
}


void ViewportHandler::updatePrevInteraction() {
    m_mousePrevPosition = m_mousePosition;

    for (auto& button : m_mouseButtons)
        button.prevDown = button.down;

    for (auto& key : m_keys)
        key.prevDown = key.down;
}

template <typename Fn> void ViewportHandler::forEachInteractionRecipient(Fn &&fn) {
    for (auto* view : m_viewports)
        fn(view);

    if (auto* view = activeViewport())
        fn(view->activeState().drawContext);
}
} // namespace ScopeCanvas::Render::Window
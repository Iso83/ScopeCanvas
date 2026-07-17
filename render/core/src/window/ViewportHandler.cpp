#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/render/RenderBenchmark.h>
#include <ScopeCanvas/render/window/Viewport.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>

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


void ViewportHandler::setActiveViewport(std::size_t index) {
    assert(index < m_viewports.size());
    if (m_activeView == index)
        return;

    if (auto* oldActive = activeViewport())
        oldActive->m_needsRender = true;

    m_activeView = index;

    if (auto* newActive = activeViewport()) 
        newActive->m_needsRender = true;
}

bool ViewportHandler::setActiveViewport(Viewport* viewport) {
    if (activeViewport() == viewport)
        return true;

    for (std::size_t i = 0; i < m_viewports.size(); ++i) {
        if (m_viewports[i] == viewport) {
            setActiveViewport(i);
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------
// Render
//-------------------------------------------------------------------------
bool ViewportHandler::needsRender() {
    for (auto* viewport : m_viewports) {
        if (viewport == nullptr)
            continue;

        if (viewport->needsRender())
            return true;

        if (viewport->m_activeState == Viewport::InvalidState)
            continue;

        const auto& state = viewport->activeState();
        if (state.drawContext != nullptr && state.drawContext->needsRender())
            return true;
    }

    return false;
}

void ViewportHandler::draw(ScopeCanvas::Render::RenderBenchmark* benchmark) {
    if (benchmark != nullptr) {
        benchmark->draw(*this);
        return;
    }

    bool contextRenderTask = false;
    for (auto* viewport : m_viewports) {
        if (viewport == nullptr || viewport->m_activeState == Viewport::InvalidState)
            continue;

        const auto& state = viewport->activeState();
        contextRenderTask = contextRenderTask || (state.drawContext != nullptr && state.drawContext->needsRender());
    }

    for (auto* viewport : m_viewports) {
        if (viewport == nullptr)
            continue;

        viewport->draw();
    }
}

//-------------------------------------------------------------------------
// Interaction
//-------------------------------------------------------------------------
bool ViewportHandler::processMouseMove(const glm::vec2 pos) {
    if (m_mousePosition == pos)
        return false;

    m_mousePosition = pos;
    if (auto* viewport = activeViewport())
        viewport->m_needsRender = true;

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
    if (auto* viewport = activeViewport())
        viewport->m_needsRender = true;

    forEachInteractionRecipient([&](auto* target) {
        if (target != nullptr && target->handlesMouseButton())
            target->onMouseButton(button, pressed);
    });
}

void ViewportHandler::processScroll(double xOffset, double yOffset) {
    if (auto* viewport = activeViewport())
        viewport->m_needsRender = true;
    forEachInteractionRecipient([&](auto* target) {
        if (target != nullptr && target->handlesScroll())
            target->onScroll(xOffset, yOffset);
    });
}

void ViewportHandler::processKey(ScopeCanvas::Input::Key key, bool pressed) {
    if (key == ScopeCanvas::Input::Key::Unknown || keyState(key).down == pressed)
        return;

    m_keys[static_cast<std::size_t>(key)].down = pressed;
    if (auto* viewport = activeViewport())
        viewport->m_needsRender = true;

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
#pragma once

#include <array>
#include <glm/vec2.hpp>
#include <ScopeCanvas/render/window/ViewportInteraction.h>
#include <stdexcept>
#include <vector>

namespace ScopeCanvas::Input {
constexpr std::size_t MaxKeys = 512;

struct State {
    bool down{};
    bool prevDown{};

    bool pressed() const {
        return down && !prevDown;
    }
    bool released() const {
        return !down && prevDown;
    }
};
} // namespace ScopeCanvas::Input

namespace ScopeCanvas::Render::Window {
class Viewport;

class ViewportHandler {
  protected:
    //-------------------------------------------------------------------------
    // Viewports
    //-------------------------------------------------------------------------
    std::vector<Viewport*> m_viewports{};
    static constexpr std::size_t InvalidView = std::numeric_limits<std::size_t>::max();
    std::size_t m_activeView{InvalidView};

    //-------------------------------------------------------------------------
    // Interaction
    //-------------------------------------------------------------------------
    std::array<Input::State, 3> m_mouseButtons{};
    glm::vec2 m_mousePosition{};
    glm::vec2 m_mousePrevPosition{};
    std::array<Input::State, Input::MaxKeys> m_keys{};

  public:
    //-------------------------------------------------------------------------
    // Viewports
    //-------------------------------------------------------------------------
    void registerViewport(Viewport* viewport);
    void unregisterViewport(Viewport* viewport);

    inline void setActiveViewport(std::size_t index) {
        assert(index < m_viewports.size());
        m_activeView = index;
    }

    inline [[nodiscard]] Viewport* activeViewport() noexcept {
        if (m_activeView >= m_viewports.size())
            return nullptr;

        return m_viewports[m_activeView];
    }

    inline [[nodiscard]] const Viewport* activeViewport() const noexcept {
        if (m_activeView >= m_viewports.size())
            return nullptr;

        return m_viewports[m_activeView];
    }

    [[nodiscard]] const std::vector<Viewport*>& viewports() const noexcept {
        return m_viewports;
    }

    //-------------------------------------------------------------------------
    // Interaction
    //-------------------------------------------------------------------------

    virtual bool processMouseMove(glm::vec2 pos);
    inline const glm::vec2& mousePosition() const {
        return m_mousePosition;
    }
    inline glm::vec2 mouseDeltaPosition() const {
        return m_mousePosition - m_mousePrevPosition; 
    }

    virtual void processMouseButton(MouseButton button, bool pressed);
    inline const Input::State mouseState(MouseButton button) const {
        if (button < 0 || static_cast<std::size_t>(button) >= m_mouseButtons.size())
            throw std::out_of_range("ViewportHandler::mouseState(): index outside supported range");

        return m_mouseButtons[button];
    }

    virtual void processScroll(double xOffset, double yOffset);

    virtual void processKey(int key, bool pressed);
    inline const Input::State keyState(int key) const {
        if (key < 0 || static_cast<std::size_t>(key) >= m_keys.size())
            throw std::out_of_range("ViewportHandler::keyState(): index outside supported range");

        return m_keys[key];
    }


    virtual void updatePrevInteraction();

  protected:
    template <typename Fn> void forEachInteractionRecipient(Fn&& fn);
};
} // namespace ScopeCanvas::Render::Window
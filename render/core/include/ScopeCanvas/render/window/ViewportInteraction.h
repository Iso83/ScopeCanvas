#pragma once

#include <ScopeCanvas/input/InputCodes.h>
#include <glm/vec2.hpp>

namespace ScopeCanvas::Render::Window {
enum MouseButton { SC_MOUSE_BUTTON_LEFT, SC_MOUSE_BUTTON_MIDDLE, SC_MOUSE_BUTTON_RIGHT };

class ViewportInteraction {
  public:
    virtual ~ViewportInteraction() = default;

#define SC_VIEWPORT_EVENT(name, ...)                                                                                   \
    virtual bool handles##name() const {                                                                               \
        return false;                                                                                                  \
    }                                                                                                                  \
    virtual void on##name(__VA_ARGS__) {}

    SC_VIEWPORT_EVENT(MouseMove, glm::vec2 pos)
    SC_VIEWPORT_EVENT(MouseButton, MouseButton button, bool pressed)
    SC_VIEWPORT_EVENT(Scroll, double xOffset, double yOffset)
    SC_VIEWPORT_EVENT(Key, ScopeCanvas::Input::Key key, bool pressed)

#undef SC_VIEWPORT_EVENT
};
} // namespace ScopeCanvas::Render::Window
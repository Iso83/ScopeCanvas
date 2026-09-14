#pragma once

#include <imgui.h>

struct GLFWwindow;

namespace ScopeCanvas::Integration::ImGui {
struct DisplayMetrics {
    float contentScaleX{1.0F};
    float contentScaleY{1.0F};
    int windowWidth{0};
    int windowHeight{0};
    int framebufferWidth{0};
    int framebufferHeight{0};
};

[[nodiscard]] float effectiveDisplayScale(const DisplayMetrics& metrics);

class DisplayScale {
    ImGuiStyle m_unscaledStyle;
    float m_scale{1.0F};

public:
    explicit DisplayScale(const ImGuiStyle& unscaledStyle);
    DisplayScale();

    [[nodiscard]] float scale() const {
        return m_scale;
    }

    bool update(const DisplayMetrics& metrics, ImGuiStyle& style, ImGuiIO& io);
    bool update(GLFWwindow* window);
};
} // namespace ScopeCanvas::Integration::ImGui

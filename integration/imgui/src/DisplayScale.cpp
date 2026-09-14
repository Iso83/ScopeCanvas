#include <GLFW/glfw3.h>
#include <ScopeCanvas/integration/imgui/DisplayScale.h>
#include <algorithm>
#include <cmath>

namespace ScopeCanvas::Integration::ImGui {
namespace Intern {
float validScale(float value) {
    return std::isfinite(value) && value > 0.0F ? std::max(value, 1.0F) : 1.0F;
}

float dimensionScale(int framebufferDimension, int windowDimension) {
    if (framebufferDimension <= 0 || windowDimension <= 0)
        return 1.0F;

    return validScale(static_cast<float>(framebufferDimension) / static_cast<float>(windowDimension));
}

DisplayMetrics queryDisplayMetrics(GLFWwindow* window) {
    DisplayMetrics metrics;
    glfwGetWindowContentScale(window, &metrics.contentScaleX, &metrics.contentScaleY);
    glfwGetWindowSize(window, &metrics.windowWidth, &metrics.windowHeight);
    glfwGetFramebufferSize(window, &metrics.framebufferWidth, &metrics.framebufferHeight);
    return metrics;
}
} // namespace Intern

float effectiveDisplayScale(const DisplayMetrics& metrics) {
    // Platforms differ in whether content scale or framebuffer density describes
    // the effective UI scale, so prefer the larger valid value without downscaling.
    return std::max({Intern::validScale(metrics.contentScaleX), Intern::validScale(metrics.contentScaleY),
                     Intern::dimensionScale(metrics.framebufferWidth, metrics.windowWidth),
                     Intern::dimensionScale(metrics.framebufferHeight, metrics.windowHeight)});
}

DisplayScale::DisplayScale(const ImGuiStyle& unscaledStyle) : m_unscaledStyle(unscaledStyle) {}

DisplayScale::DisplayScale() : DisplayScale(::ImGui::GetStyle()) {}

bool DisplayScale::update(const DisplayMetrics& metrics, ImGuiStyle& style, ImGuiIO& io) {
    const auto nextScale = effectiveDisplayScale(metrics);
    if (std::abs(nextScale - m_scale) < 0.01F)
        return false;

    m_scale = nextScale;
    io.FontGlobalScale = m_scale;
    style = m_unscaledStyle;
    style.ScaleAllSizes(m_scale);
    return true;
}

bool DisplayScale::update(GLFWwindow* window) {
    return update(Intern::queryDisplayMetrics(window), ::ImGui::GetStyle(), ::ImGui::GetIO());
}
} // namespace ScopeCanvas::Integration::ImGui

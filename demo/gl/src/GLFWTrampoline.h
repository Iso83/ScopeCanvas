#pragma once

#include <GLFW/glfw3.h>

namespace sc::glfw {
// Framebuffer size
template <typename T, void (T::*Method)(int, int)>
static void framebufferSize(GLFWwindow* window, int width, int height) {
    if (auto* instance = static_cast<T*>(glfwGetWindowUserPointer(window)))
        (instance->*Method)(width, height);
}

// Cursor position
template <typename T, void (T::*Method)(double, double)> static void cursorPos(GLFWwindow* window, double x, double y) {
    if (auto* instance = static_cast<T*>(glfwGetWindowUserPointer(window)))
        (instance->*Method)(x, y);
}

// Mouse button
template <typename T, void (T::*Method)(int, int, int)>
static void mouseButton(GLFWwindow* window, int button, int action, int mods) {
    if (auto* instance = static_cast<T*>(glfwGetWindowUserPointer(window)))
        (instance->*Method)(button, action, mods);
}

// Scroll
template <typename T, void (T::*Method)(double, double)>
static void scroll(GLFWwindow* window, double xoffset, double yoffset) {
    if (auto* instance = static_cast<T*>(glfwGetWindowUserPointer(window)))
        (instance->*Method)(xoffset, yoffset);
}
} // namespace sc::glfw
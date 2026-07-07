#pragma once

#include <ScopeCanvas/glfw/GlfwInputListener.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ScopeCanvas::Demo {
class ImGuiInputListener : public GLFW::GlfwInputListener {
  private:
    static ImGuiKey glfwToImGuiKey(int key) {
        switch (key) {
        case GLFW_KEY_TAB:
            return ImGuiKey_Tab;
        case GLFW_KEY_LEFT:
            return ImGuiKey_LeftArrow;
        case GLFW_KEY_RIGHT:
            return ImGuiKey_RightArrow;
        case GLFW_KEY_UP:
            return ImGuiKey_UpArrow;
        case GLFW_KEY_DOWN:
            return ImGuiKey_DownArrow;
        case GLFW_KEY_PAGE_UP:
            return ImGuiKey_PageUp;
        case GLFW_KEY_PAGE_DOWN:
            return ImGuiKey_PageDown;
        case GLFW_KEY_HOME:
            return ImGuiKey_Home;
        case GLFW_KEY_END:
            return ImGuiKey_End;
        case GLFW_KEY_INSERT:
            return ImGuiKey_Insert;
        case GLFW_KEY_DELETE:
            return ImGuiKey_Delete;
        case GLFW_KEY_BACKSPACE:
            return ImGuiKey_Backspace;
        case GLFW_KEY_SPACE:
            return ImGuiKey_Space;
        case GLFW_KEY_ENTER:
            return ImGuiKey_Enter;
        case GLFW_KEY_ESCAPE:
            return ImGuiKey_Escape;

        case GLFW_KEY_A:
            return ImGuiKey_A;
        case GLFW_KEY_B:
            return ImGuiKey_B;
        case GLFW_KEY_C:
            return ImGuiKey_C;
            // ... later de rest aanvullen indien nodig

        case GLFW_KEY_LEFT_CONTROL:
            return ImGuiKey_LeftCtrl;
        case GLFW_KEY_LEFT_SHIFT:
            return ImGuiKey_LeftShift;
        case GLFW_KEY_LEFT_ALT:
            return ImGuiKey_LeftAlt;
        case GLFW_KEY_LEFT_SUPER:
            return ImGuiKey_LeftSuper;

        case GLFW_KEY_RIGHT_CONTROL:
            return ImGuiKey_RightCtrl;
        case GLFW_KEY_RIGHT_SHIFT:
            return ImGuiKey_RightShift;
        case GLFW_KEY_RIGHT_ALT:
            return ImGuiKey_RightAlt;
        case GLFW_KEY_RIGHT_SUPER:
            return ImGuiKey_RightSuper;

        default:
            return ImGuiKey_None;
        }
    }

  public:
    inline void cursorPosCallback(GLFWwindow* w, double x, double y) override {
        ImGui_ImplGlfw_CursorPosCallback(w, x, y);
    }

    inline void mouseButtonCallback(GLFWwindow* w, int button, int action, int mods) override {
        ImGui_ImplGlfw_MouseButtonCallback(w, button, action, mods);
    }

    inline void scrollCallback(GLFWwindow* w, double xOffset, double yOffset) override {
        ImGui_ImplGlfw_ScrollCallback(w, xOffset, yOffset);
    }

    inline void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods) override {
        ImGui_ImplGlfw_KeyCallback(w, key, scancode, action, mods);
    }
};
}
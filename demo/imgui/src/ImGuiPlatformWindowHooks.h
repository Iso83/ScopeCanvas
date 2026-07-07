#pragma once

#include "imgui.h"

#include <ScopeCanvas/glfw/GlfwInputListener.h>
#include <ScopeCanvas/glfw/GlfwViewportBindings.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ScopeCanvas::Demo {
class ImGuiPlatformWindowHooks {
  private:
    using CreateWindowCallback = void (*)(ImGuiViewport* viewport);
    using DestroyWindowCallback = void (*)(ImGuiViewport* viewport);

    inline static CreateWindowCallback s_previousCreateWindow{};
    inline static DestroyWindowCallback s_previousDestroyWindow{};
    inline static GLFW::GlfwInputListener* s_listener{};
    inline static Render::Window::ViewportHandler* s_viewportHandler{};

    static GLFWwindow* glfwWindowForViewport(ImGuiViewport* viewport) {
        return static_cast<GLFWwindow*>(viewport->PlatformHandle);
    }

    static void attachCallbacks(GLFWwindow* window) {
        if (!window)
            return;

        GLFW::bindInputListener(window, s_listener);

        if (s_viewportHandler)
            GLFW::bindViewportHandler(window, s_viewportHandler);
        else
            GLFW::installCallbacks(window);
    }

    static void createWindow(ImGuiViewport* viewport) {
        if (s_previousCreateWindow)
            s_previousCreateWindow(viewport);

        attachCallbacks(glfwWindowForViewport(viewport));
    }

    static void detachCallbacks(GLFWwindow* window) {
        if (!window)
            return;

        GLFW::bindInputListener(window, nullptr);
        GLFW::bindViewportHandler(window, nullptr);
    }

    static void destroyWindow(ImGuiViewport* viewport) {
        detachCallbacks(glfwWindowForViewport(viewport));

        if (s_previousDestroyWindow)
            s_previousDestroyWindow(viewport);
    }

  public:
    static void install(GLFWwindow* mainWindow, GLFW::GlfwInputListener* listener,
                        Render::Window::ViewportHandler* viewportHandler) {
        s_listener = listener;
        s_viewportHandler = viewportHandler;

        attachCallbacks(mainWindow);

        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        s_previousCreateWindow = platformIO.Platform_CreateWindow;
        s_previousDestroyWindow = platformIO.Platform_DestroyWindow;
        platformIO.Platform_CreateWindow = createWindow;
        platformIO.Platform_DestroyWindow = destroyWindow;
    }
};
} // namespace ScopeCanvas::Demo

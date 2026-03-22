#include "diagram/DiagramBasics.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "windows/NodeDiagramWindow.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace ScopeCanvas::Studio;

namespace {
int RunStudioApp() {
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1400, 900, "ScopeCanvas Studio", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    DiagramBasics basics;
    ViewState viewA{0.0F, 0.0F, 1.0F};
    ViewState viewB{180.0F, -80.0F, 0.9F};

    NodeDiagramWindow canvasA(window, &basics, &viewA, "Canvas A");
    NodeDiagramWindow canvasB(window, &basics, &viewB, "Canvas B");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Create")) {
                if (ImGui::MenuItem("Number"))
                    basics.createNode(ScopeCanvas::Core::NodeTypeId{1}, {-120.0F, 0.0F});
                if (ImGui::MenuItem("Add"))
                    basics.createNode(ScopeCanvas::Core::NodeTypeId{2}, {80.0F, 0.0F});
                if (ImGui::MenuItem("Multiply"))
                    basics.createNode(ScopeCanvas::Core::NodeTypeId{3}, {280.0F, 0.0F});
                if (ImGui::MenuItem("Output"))
                    basics.createNode(ScopeCanvas::Core::NodeTypeId{4}, {480.0F, 0.0F});
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Show Grid", nullptr, &basics.gridSettings().enabled);
                ImGui::MenuItem("Snap to Grid", nullptr, &basics.gridSettings().snapEnabled);
                ImGui::Separator();
                ImGui::TextUnformatted("Use the per-canvas panel for grid/debug toggles.");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        canvasA.draw();
        canvasB.draw();

        ImGui::Render();
        int w = 0;
        int h = 0;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
} // namespace

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return RunStudioApp();
}
#else
int main() {
    return RunStudioApp();
}
#endif

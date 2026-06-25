#include <ScopeCanvas/render/window/Canvas.h>
#include <ScopeCanvas/demo/DiagramDrawContext.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace ScopeCanvas::Render::Window;
using namespace ScopeCanvas::Demo;

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#else
int main() {
#endif

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width(1400), height(900);
    GLFWwindow* window = glfwCreateWindow(width, height, "ScopeCanvas Docking Demo", nullptr, nullptr);
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

    DiagramDrawCtx drawCtx{};
    Canvas canvasA;
    Canvas canvasB;
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Create")) {
                if (ImGui::MenuItem("Number"))
                    drawCtx.Doc().createNode(ScopeCanvas::Core::Ids::NodeTypeId{1}, {-120.0F, 0.0F});
                if (ImGui::MenuItem("Add"))
                    drawCtx.Doc().createNode(ScopeCanvas::Core::Ids::NodeTypeId{2}, {80.0F, 0.0F});
                if (ImGui::MenuItem("Multiply"))
                    drawCtx.Doc().createNode(ScopeCanvas::Core::Ids::NodeTypeId{3}, {280.0F, 0.0F});
                if (ImGui::MenuItem("Output"))
                    drawCtx.Doc().createNode(ScopeCanvas::Core::Ids::NodeTypeId{4}, {480.0F, 0.0F});
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Delete Selection"))
                    drawCtx.deleteSelection();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Show Grid", nullptr, &drawCtx.showGrid());
                ImGui::Separator();
                ImGui::TextUnformatted("Use the per-canvas panel for grid/debug toggles.");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        auto drawCanvasPanel = [&drawCtx](Canvas& canvas, const char *title) {
            ImGui::Begin(title);
            /*ImGui::Checkbox("Grid", &canvas.showGrid());
            ImGui::SameLine();
            ImGui::Checkbox("Debug", &canvas.showDebug());*/

            const ImVec2 canvasPos = ImGui::GetCursorScreenPos();
            const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
            const ImVec2 mouse = ImGui::GetIO().MousePos;
            const bool hovered = ImGui::IsWindowHovered() || ImGui::IsItemHovered();

            DiagramInput input{};
            input.mouseX = mouse.x - canvasPos.x;
            input.mouseY = mouse.y - canvasPos.y;
            input.hovered = hovered;
            input.leftDown = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
            input.leftPressed = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
            input.leftReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
            input.middleDown = hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Middle);
            input.mouseDelta = {ImGui::GetIO().MouseDelta.x, ImGui::GetIO().MouseDelta.y};
            input.scrollDelta = hovered ? ImGui::GetIO().MouseWheel : 0.0F;
            input.deletePressed = hovered && ImGui::IsKeyPressed(ImGuiKey_Delete);
            drawCtx.updateInput(input);

            canvas.setViewportSize(canvasSize.x, canvasSize.y);
            canvas.draw(&drawCtx);

            ImGui::Image(static_cast<ImTextureID>(canvas.colorTexture()), canvasSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();
        };

        drawCanvasPanel(canvasA, "Primary Canvas");
        drawCanvasPanel(canvasB, "Secondary Canvas");

        ImGui::Render();
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
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

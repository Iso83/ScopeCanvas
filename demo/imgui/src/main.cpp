#ifdef SC_BUILD_DEMO_BENCHMARK
#include <ScopeCanvas/render/RenderBenchmark.h>
#endif

#include "ImGuiInputListener.h"
#include "ImGuiPlatformWindowHooks.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <ScopeCanvas/demo/DiagramDrawContext.h>
#include <ScopeCanvas/glfw/GlfwBootstrap.h>
#include <ScopeCanvas/glfw/GlfwViewportBindings.h>
#include <ScopeCanvas/render/window/Canvas.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>

using namespace ScopeCanvas::Render;
using namespace ScopeCanvas::Render::Window;
using namespace ScopeCanvas::Demo;
using namespace ScopeCanvas::GLFW;

#ifdef SC_BUILD_DEMO_BENCHMARK
#define SWAPINTERVAL 0
#else
#define SWAPINTERVAL 1
#endif

static void initializeImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

static void drawMainMenu(DiagramDrawCtx& drawCtx) {
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
}

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#else
int main() {
#endif

    GLFWwindow* window = createOpenGLWindow(1280, 720, "ScopeCanvas Docking Demo", true, SWAPINTERVAL);
    if (window == nullptr)
        return -1;

    initializeImGui(window);

    DiagramDrawCtx drawCtx{};

    Canvas viewA, viewB;
    viewA.registerDrawContext(&drawCtx);
    viewA.setViewPosition({120.0F, 0.0F});
    viewB.registerDrawContext(&drawCtx);

    ViewportHandler viewHandler;
    viewHandler.registerViewport(&viewA);
    viewHandler.registerViewport(&viewB);

    ImGuiInputListener listener{};
    ImGuiPlatformWindowHooks::install(window, &listener, &viewHandler);

#ifdef SC_BUILD_DEMO_BENCHMARK
    RenderBenchmark benchmark{};
#endif

    bool needsPresent{true};
    while (!glfwWindowShouldClose(window)) {
        if (!needsPresent) {
            glfwWaitEvents();
            needsPresent = true;
        } else
            glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        drawMainMenu(drawCtx);

        // TODO: Remove after ViewportInteraction refactor.
        // Benchmark::draw(viewHandler) renders every registered viewport.
        // Calling it from the per-canvas lambda causes redundant renders.
        auto drawCanvasPanel = [&drawCtx, &needsPresent, &viewHandler, &window
#ifdef SC_BUILD_DEMO_BENCHMARK
                                ,
                                &benchmark
#endif
        ](Canvas& canvas, const char* title) {
            ImGui::Begin(title);
            /*ImGui::Checkbox("Grid", &canvas.showGrid());
            ImGui::SameLine();
            ImGui::Checkbox("Debug", &canvas.showDebug());*/

            GLFWwindow* currentWindow = window;
            if (ImGuiViewport* imguiViewport = ImGui::GetWindowViewport()) {
                if (auto* viewportWindow = static_cast<GLFWwindow*>(imguiViewport->PlatformHandle))
                    currentWindow = viewportWindow;
            }

            int wx, wy;
            glfwGetWindowPos(currentWindow, &wx, &wy);

            ImVec2 screen = ImGui::GetCursorScreenPos();
            glm::vec2 local{screen.x - wx, screen.y - wy};
            canvas.setScreenPosition(local);

            const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
            const bool hovered = ImGui::IsWindowHovered() || ImGui::IsItemHovered();

            if (hovered) {
                if (viewHandler.activeViewport() != &canvas) {
                    auto viewports = viewHandler.viewports();
                    for (int i = 0; i < viewports.size(); i++) {
                        if (viewports[i] == &canvas) {
                            viewHandler.setActiveViewport(i);
                            break;
                        }
                    }
                }
            }
            canvas.setViewportSize(canvasSize.x, canvasSize.y);

            // if (canvas.needsRender()) {
#ifdef SC_BUILD_DEMO_BENCHMARK
            benchmark.draw(viewHandler);
#else
            if (hovered)
                canvas.draw();
#endif
            needsPresent = true;
            //}

            ImGui::Image(static_cast<ImTextureID>(canvas.colorTexture()), canvasSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();
        };

        drawCanvasPanel(viewA, "Primary Canvas");
        drawCanvasPanel(viewB, "Secondary Canvas");

#ifdef SC_BUILD_DEMO_BENCHMARK
        const auto& stats = benchmark.statistics();
        ImGui::Begin("Render Benchmark");
        ImGui::Text("Frames: %llu", stats.renderedFrames);
        ImGui::Text("Elapsed: %.3f s", stats.elapsedSeconds);
        ImGui::Text("FPS: %.2f", stats.framesPerSecond);
        ImGui::Text("Average frame: %.3f ms", stats.averageFrameTimeMs);
        ImGui::End();

#endif

        ImGui::Render();

        prepareFramebuffer(window);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);
        }

        viewHandler.updatePrevInteraction();
        glfwSwapBuffers(window);
        needsPresent = false;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
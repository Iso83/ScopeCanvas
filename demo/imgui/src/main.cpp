#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiInputListener.h"
#include "ImGuiPlatformWindowHooks.h"

#include <ScopeCanvas/demo/DiagramDrawContext.h>
#include <ScopeCanvas/glfw/GlfwBootstrap.h>
#include <ScopeCanvas/render/RenderBenchmark.h>
#include <ScopeCanvas/render/window/Canvas.h>

using namespace ScopeCanvas::Render;
using namespace ScopeCanvas::Render::Window;
using namespace ScopeCanvas::Demo;
using namespace ScopeCanvas::GLFW;

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
                drawCtx.document().createNode(ScopeCanvas::Core::Ids::NodeTypeId{1}, {-120.0F, 0.0F});

            if (ImGui::MenuItem("Add"))
                drawCtx.document().createNode(ScopeCanvas::Core::Ids::NodeTypeId{2}, {80.0F, 0.0F});

            if (ImGui::MenuItem("Multiply"))
                drawCtx.document().createNode(ScopeCanvas::Core::Ids::NodeTypeId{3}, {280.0F, 0.0F});

            if (ImGui::MenuItem("Output"))
                drawCtx.document().createNode(ScopeCanvas::Core::Ids::NodeTypeId{4}, {480.0F, 0.0F});

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Delete Selection"))
                drawCtx.deleteSelection();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Show Grid", nullptr, &drawCtx.showGrid());

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

static void beginImGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();
}

static void endImGuiFrame(GLFWwindow* window) {
    ImGui::Render();

    prepareFramebuffer(window);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
    }
}

static void shutdownImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

static void drawCanvasPanel(Canvas& canvas, const char* title, GLFWwindow* window, ViewportHandler& viewHandler,
                            bool focusOnOpen = false) {
    static int startupFocusFrames = 2;

    if (focusOnOpen && startupFocusFrames > 0) {
        ImGui::SetNextWindowFocus();
        --startupFocusFrames;
    }

    ImGui::Begin(title);

    GLFWwindow* currentWindow = window;
    if (auto* imguiViewport = ImGui::GetWindowViewport()) {
        if (auto* viewportWindow = static_cast<GLFWwindow*>(imguiViewport->PlatformHandle))
            currentWindow = viewportWindow;
    }

    int wx, wy;
    glfwGetWindowPos(currentWindow, &wx, &wy);
    ImVec2 screen = ImGui::GetCursorScreenPos();
    canvas.setScreenPosition({screen.x - wx, screen.y - wy});

    const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvas.setViewportSize(canvasSize.x, canvasSize.y);
    
    if (ImGui::IsWindowFocused())
        viewHandler.setActiveViewport(&canvas);
    else if (viewHandler.activeViewport() == &canvas)
        viewHandler.clearActiveViewport();

    ImGui::Image(static_cast<ImTextureID>(canvas.colorTexture()), canvasSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

#ifdef SC_BUILD_DEMO_BENCHMARK
static void drawBenchmarkWindow(const RenderBenchmark& benchmark) {
    const auto& stats = benchmark.statistics();

    ImGui::Begin("Render Benchmark");
    ImGui::Text("Frames: %llu", stats.renderedFrames);
    ImGui::Text("Elapsed: %.3f s", stats.elapsedSeconds);
    ImGui::Text("FPS: %.2f", stats.framesPerSecond);
    ImGui::Text("Estimated: %.2f", stats.estimatedFramesPerSecond);
    ImGui::Text("Average frame: %.3f ms", stats.averageFrameTimeMs);
    ImGui::Text("Render load: %.1f %%", stats.renderLoadPercent);
    ImGui::Text("Idle: %.1f ms", stats.idleTimeMs);
    ImGui::End();
}
#endif

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#else
int main() {
#endif

    GLFWwindow* window = createOpenGLWindow(1280, 720, "ScopeCanvas Docking Demo", true, SC_SWAPINTERVAL);
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

    while (!glfwWindowShouldClose(window)) {
        viewHandler.needsRender() ? glfwPollEvents() : glfwWaitEvents();

        beginImGuiFrame();
        drawMainMenu(drawCtx);
        drawCanvasPanel(viewA, "Primary Canvas", window, viewHandler, true);
        drawCanvasPanel(viewB, "Secondary Canvas", window, viewHandler);

#ifdef SC_BUILD_DEMO_BENCHMARK
        viewHandler.draw(&benchmark);
        drawBenchmarkWindow(benchmark);
#else
        viewHandler.draw();
#endif

        endImGuiFrame(window);
        viewHandler.updatePrevInteraction();
        glfwSwapBuffers(window);
    }

    shutdownImGui();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
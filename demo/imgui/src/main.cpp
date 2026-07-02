#ifdef SC_BUILD_DEMO_BENCHMARK
#include <ScopeCanvas/render/RenderBenchmark.h>
#endif
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <ScopeCanvas/demo/DiagramDrawContext.h>
#include <ScopeCanvas/render/window/Canvas.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace ScopeCanvas::Render::Window;
using namespace ScopeCanvas::Demo;

ViewportHandler viewHandler;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_REPEAT)
        return;
    viewHandler.processKey(key, action == GLFW_PRESS);
}

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

#ifndef SC_BUILD_DEMO_BENCHMARK
    glfwSwapInterval(1);
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    DiagramDrawCtx drawCtx{};

    Canvas viewA;
    viewA.registerDrawContext(&drawCtx);
    Canvas viewB;
    viewB.registerDrawContext(&drawCtx);

    
    viewHandler.registerViewport(&viewA);
    viewHandler.registerViewport(&viewB);

     glfwSetKeyCallback(window, keyCallback);


#ifdef SC_BUILD_DEMO_BENCHMARK
    ScopeCanvas::Render::RenderBenchmark benchmark{};
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

     

        // TODO: Remove after ViewportInteraction refactor.
        // Benchmark::draw(viewHandler) renders every registered viewport.
        // Calling it from the per-canvas lambda causes redundant renders.
        auto drawCanvasPanel = [ &drawCtx, &needsPresent
#ifdef SC_BUILD_DEMO_BENCHMARK
                                ,
                                &benchmark
#endif
        ](Canvas& canvas, const char* title) {
            ImGui::Begin(title);
            /*ImGui::Checkbox("Grid", &canvas.showGrid());
            ImGui::SameLine();
            ImGui::Checkbox("Debug", &canvas.showDebug());*/

            const ImVec2 canvasPos = ImGui::GetCursorScreenPos();
            const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
            const ImVec2 mouse = ImGui::GetIO().MousePos;
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

                viewHandler.processMouseMove({mouse.x - canvasPos.x, mouse.y - canvasPos.y});
                
                viewHandler.processMouseButton(SC_MOUSE_BUTTON_LEFT, ImGui::IsMouseDown(ImGuiMouseButton_Left));
                viewHandler.processMouseButton(SC_MOUSE_BUTTON_MIDDLE, ImGui::IsMouseDown(ImGuiMouseButton_Middle));
                viewHandler.processMouseButton(SC_MOUSE_BUTTON_RIGHT, ImGui::IsMouseDown(ImGuiMouseButton_Right));

                auto mouseWheel = ImGui::GetIO().MouseWheel;
                if (mouseWheel != 0.0F)
                    viewHandler.processScroll(0, mouseWheel);
            }

            canvas.setViewportSize(canvasSize.x, canvasSize.y);

            //if (canvas.needsRender()) {
#ifdef SC_BUILD_DEMO_BENCHMARK
                benchmark.draw(viewHandler);
#else
                canvas.draw();
#endif
                needsPresent = true;
            //}
                

            ImGui::Image(static_cast<ImTextureID>(canvas.colorTexture()), canvasSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();
        };

        drawCanvasPanel(viewA, "Primary Canvas");
        //drawCanvasPanel(viewB, "Secondary Canvas");

        viewHandler.updatePrevInteraction();
        

#ifdef SC_BUILD_DEMO_BENCHMARK
        const auto& stats = benchmark.statistics();
        ImGui::Begin("Render Benchmark");
        ImGui::Text("Frames: %llu", static_cast<unsigned long long>(stats.renderedFrames));
        ImGui::Text("Elapsed: %.3f s", stats.elapsedSeconds);
        ImGui::Text("FPS: %.2f", stats.framesPerSecond);
        ImGui::Text("Average frame: %.3f ms", stats.averageFrameTimeMs);
        ImGui::End();

#endif

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
        needsPresent = false;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
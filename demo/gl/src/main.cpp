#ifdef SC_BUILD_DEMO_BENCHMARK
#include <ScopeCanvas/render/RenderBenchmark.h>
#endif
#include <ScopeCanvas/render/window/Viewport.h>
#include <ScopeCanvas/demo/DiagramDrawContext.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

using namespace ScopeCanvas::Render::Window;
using namespace ScopeCanvas::Demo;

namespace {
struct GlInputState {
    double mouseX{0.0};
    double mouseY{0.0};
    double previousMouseX{0.0};
    double previousMouseY{0.0};
    bool leftDown{false};
    bool previousLeftDown{false};
    bool middleDown{false};
    float scrollDelta{0.0F};
    bool deletePressed{false};
    bool previousDeleteDown{false};
};

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    auto* input = static_cast<GlInputState*>(glfwGetWindowUserPointer(window));
    input->mouseX = x;
    input->mouseY = y;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;
    auto* input = static_cast<GlInputState*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        input->leftDown = action == GLFW_PRESS;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        input->middleDown = action == GLFW_PRESS;
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    (void)xOffset;
    auto* input = static_cast<GlInputState*>(glfwGetWindowUserPointer(window));
    input->scrollDelta += static_cast<float>(yOffset);
}
} // namespace

int main() {
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ScopeCanvas OpenGL Demo", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
#ifndef SC_BUILD_DEMO_BENCHMARK
    glfwSwapInterval(1);
#endif

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize OpenGL loader\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    GlInputState inputState{};
    glfwSetWindowUserPointer(window, &inputState);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);

    DiagramDrawCtx drawCtx{};
    Viewport view{};
    view.setViewPosition({120.0F, 0.0F});

#ifdef SC_BUILD_DEMO_BENCHMARK
    ScopeCanvas::Render::RenderBenchmark benchmark{};
    benchmark.registerViewport(&view);
#endif
   
    float lastTime = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        const float now = static_cast<float>(glfwGetTime());
        const float deltaTime = now - lastTime;
        lastTime = now;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        constexpr float panSpeed = 500.0F;
        glm::vec2 panDelta{0.0F, 0.0F};
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            panDelta.x -= panSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            panDelta.x += panSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            panDelta.y += panSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            panDelta.y -= panSpeed * deltaTime;
        view.moveView(panDelta);

        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
            drawCtx.showGrid() = true;
        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
            drawCtx.showDebug() = true;

        static bool createHandled[4]{};
        const int keys[] = {GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4};
        const ScopeCanvas::Core::Ids::NodeTypeId types[] = {
            ScopeCanvas::Core::Ids::NodeTypeId{1}, ScopeCanvas::Core::Ids::NodeTypeId{2},
            ScopeCanvas::Core::Ids::NodeTypeId{3}, ScopeCanvas::Core::Ids::NodeTypeId{4}};
        for (int i = 0; i < 4; ++i) {
            const bool down = glfwGetKey(window, keys[i]) == GLFW_PRESS;
            if (down && !createHandled[i])
                (void)drawCtx.createNodeAtCenter(view.camera(), types[i]);
            createHandled[i] = down;
        }

        const bool deleteDown = glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS;
        inputState.deletePressed = deleteDown && !inputState.previousDeleteDown;
        inputState.previousDeleteDown = deleteDown;

        int width = 1;
        int height = 1;
        glfwGetFramebufferSize(window, &width, &height);
        view.setViewportSize(width, height);
        

        DiagramInput input{};
        input.mouseX = static_cast<float>(inputState.mouseX);
        input.mouseY = static_cast<float>(inputState.mouseY);
        input.hovered = true;
        input.leftDown = inputState.leftDown;
        input.leftPressed = inputState.leftDown && !inputState.previousLeftDown;
        input.leftReleased = !inputState.leftDown && inputState.previousLeftDown;
        input.middleDown = inputState.middleDown;
        input.mouseDelta = {static_cast<float>(inputState.mouseX - inputState.previousMouseX),
                            static_cast<float>(inputState.mouseY - inputState.previousMouseY)};
        input.scrollDelta = inputState.scrollDelta;
        input.deletePressed = inputState.deletePressed;
        drawCtx.updateInput(input);

#ifdef SC_BUILD_DEMO_BENCHMARK
        benchmark.draw(view, drawCtx);

        if (benchmark.updated()) {
            const auto& stats = benchmark.statistics();
            std::cout << "Render benchmark: frames=" << stats.renderedFrames
                      << " elapsed=" << stats.elapsedSeconds << "s"
                      << " fps=" << stats.framesPerSecond
                      << " avg=" << stats.averageFrameTimeMs << "ms\n";
        }
#else
        view.draw(&drawCtx);
#endif

        inputState.previousMouseX = inputState.mouseX;
        inputState.previousMouseY = inputState.mouseY;
        inputState.previousLeftDown = inputState.leftDown;
        inputState.scrollDelta = 0.0F;
        inputState.deletePressed = false;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

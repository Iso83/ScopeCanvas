#ifdef SC_BUILD_DEMO_BENCHMARK
#include <ScopeCanvas/render/RenderBenchmark.h>
#endif
#include <glad/glad.h>
#include <ScopeCanvas/demo/DiagramDrawContext.h>
#include <ScopeCanvas/render/window/Viewport.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

using namespace ScopeCanvas::Render::Window;
using namespace ScopeCanvas::Demo;

ViewportHandler viewHandler;

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    viewHandler.processMouseMove({x, y});
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_REPEAT)
        return;

    bool pressed{action == GLFW_PRESS};

    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        viewHandler.processMouseButton(SC_MOUSE_BUTTON_LEFT, pressed);
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        viewHandler.processMouseButton(SC_MOUSE_BUTTON_MIDDLE, pressed);
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        viewHandler.processMouseButton(SC_MOUSE_BUTTON_RIGHT, pressed);
        break;
    }
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    viewHandler.processScroll(xOffset, yOffset);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_REPEAT) return;
    viewHandler.processKey(key, action == GLFW_PRESS);
}

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


    DiagramDrawCtx drawCtx{};

    Viewport view{};
    view.registerDrawContext(&drawCtx);
    view.setViewPosition({120.0F, 0.0F});
    
    viewHandler.registerViewport(&view);

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

#ifdef SC_BUILD_DEMO_BENCHMARK
    ScopeCanvas::Render::RenderBenchmark benchmark{};
#endif

    bool needsPresent{true};
    float lastTime = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        if (!needsPresent) {
            glfwWaitEvents();
            needsPresent = true;
        } else
            glfwPollEvents();

        const float now = static_cast<float>(glfwGetTime());
        const float deltaTime = now - lastTime;
        lastTime = now;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        constexpr float panSpeed = 500.0F;
        glm::vec2 panDelta{0.0F, 0.0F};


        if (viewHandler.keyState(GLFW_KEY_A).down)
            panDelta.x -= panSpeed * deltaTime;
        if (viewHandler.keyState(GLFW_KEY_D).down)
            panDelta.x += panSpeed * deltaTime;
        if (viewHandler.keyState(GLFW_KEY_W).down)
            panDelta.y += panSpeed * deltaTime;
        if (viewHandler.keyState(GLFW_KEY_S).down)
            panDelta.y -= panSpeed * deltaTime;
        view.moveView(panDelta);
        

        if (viewHandler.keyState(GLFW_KEY_G).pressed())
            drawCtx.showGrid() = !drawCtx.showGrid();
        if (viewHandler.keyState(GLFW_KEY_F1).pressed())
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

        int width = 1;
        int height = 1;
        glfwGetFramebufferSize(window, &width, &height);
        view.setViewportSize(width, height);

#ifdef SC_BUILD_DEMO_BENCHMARK
        benchmark.draw(viewHandler);

        if (benchmark.updated()) {
            const auto& stats = benchmark.statistics();
            std::cout << "Render benchmark: frames=" << stats.renderedFrames << " elapsed=" << stats.elapsedSeconds
                      << "s"
                      << " fps=" << stats.framesPerSecond << " avg=" << stats.averageFrameTimeMs << "ms\n";
        }
#else
        view.draw();
#endif

        viewHandler.updatePrevInteraction();

        glfwSwapBuffers(window);
        needsPresent = false;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

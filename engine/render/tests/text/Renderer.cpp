#include "TestAssert.h"

#include <ScopeCanvas/engine/render/gl/OpenGLApi.h>
#include <ScopeCanvas/engine/render/text/Renderer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using ScopeCanvas::Engine::Render::Text::Renderer;

int test_font_lifecycle_and_measurement() {
    Renderer renderer;
    CPPTEST_ASSERT(renderer.init());
    CPPTEST_ASSERT(renderer.loadFont(SC_TEST_FONT_PATH, 24));
    CPPTEST_ASSERT(renderer.ready());

    const auto metrics24 = renderer.fontMetrics();
    CPPTEST_ASSERT(metrics24.pixelSize == 24);
    CPPTEST_ASSERT(metrics24.ascent > 0.0F);
    CPPTEST_ASSERT(metrics24.descent <= 0.0F);
    CPPTEST_ASSERT(metrics24.lineHeight > 0.0F);
    CPPTEST_ASSERT(renderer.measure("") == glm::vec2(0.0F));

    const auto first = renderer.measure("ScopeCanvas");
    CPPTEST_ASSERT(first.x > 0.0F);
    CPPTEST_ASSERT(renderer.measure("ScopeCanvas") == first);

    CPPTEST_ASSERT(renderer.loadFont(SC_TEST_FONT_PATH, 48));
    CPPTEST_ASSERT(renderer.fontMetrics().pixelSize == 48);
    CPPTEST_ASSERT(renderer.measure("ScopeCanvas").x > first.x);

    renderer.releaseFont();
    CPPTEST_ASSERT(!renderer.ready());
    CPPTEST_ASSERT(renderer.measure("ScopeCanvas") == glm::vec2(0.0F));

    CPPTEST_ASSERT(renderer.loadFont(SC_TEST_FONT_PATH, 18));
    CPPTEST_ASSERT(renderer.ready());

    renderer.shutdown();
    CPPTEST_ASSERT(!renderer.ready());

    return 0;
}

int test_repeated_shutdown_is_safe() {
    Renderer renderer;
    CPPTEST_ASSERT(renderer.init());
    CPPTEST_ASSERT(renderer.loadFont(SC_TEST_FONT_PATH, 24));
    CPPTEST_ASSERT(renderer.ready());

    renderer.shutdown();
    CPPTEST_ASSERT(!renderer.ready());

    // Shutdown must be idempotent.
    renderer.shutdown();
    CPPTEST_ASSERT(!renderer.ready());

    return 0;
}

int main() {
    CPPTEST_ASSERT(glfwInit());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(64, 64, "TextRenderer test", nullptr, nullptr);
    CPPTEST_ASSERT(window != nullptr);

    glfwMakeContextCurrent(window);
    CPPTEST_ASSERT(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) != 0);

    CPPTEST_RUN(test_font_lifecycle_and_measurement);
    CPPTEST_RUN(test_repeated_shutdown_is_safe);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

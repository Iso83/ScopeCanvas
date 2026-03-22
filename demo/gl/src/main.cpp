#include "App.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

int main() {
    App app;
    if (!app.init())
        return -1;

    app.run();

    glfwTerminate();
    return 0;
}
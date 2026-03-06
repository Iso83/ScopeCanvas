#pragma once

struct GLFWwindow;

#include "Engine/DiagramModel.h"
#include "Render/Renderer.h"

class App {
public:
    App();
    ~App();

    bool init();
    void run();

private:
    void shutdown();
    void processInput(float deltaTime);

    GLFWwindow* m_window;
    Renderer m_renderer;
    DiagramModel m_model;
    bool m_initialized;
};

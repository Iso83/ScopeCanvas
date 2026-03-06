#pragma once

#include "Engine/DiagramModel.h"
#include "Render/Renderer.h"
#include "View/Camera2D.h"

class App {
public:
    bool init();
    void run();

private:
    Engine::DiagramModel m_model;
    View::Camera2D m_camera;
    Render::Renderer m_renderer;
};

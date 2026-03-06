#pragma once

#include "Engine/DiagramModel.h"
#include "Interaction/DragController.h"
#include "Interaction/SelectionController.h"
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
    Interaction::SelectionController m_selectionController;
    Interaction::DragController m_dragController;
};

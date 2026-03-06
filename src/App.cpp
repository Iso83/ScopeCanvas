#include "App.h"

bool App::init() {
    m_camera.pan(-50.0F, -30.0F);
    m_camera.setZoom(1.0F);
    return true;
}

void App::run() {
    m_selectionController.selectNodeAt(m_model, Engine::Vec2 {10.0F, 10.0F});
    if (m_dragController.beginDrag(m_model, Engine::Vec2 {10.0F, 10.0F})) {
        m_dragController.updateDrag(m_model, Engine::Vec2 {80.0F, 60.0F});
        m_dragController.endDrag();
    }

    m_renderer.render(m_model, m_camera);
}

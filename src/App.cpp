#include "App.h"

bool App::init() {
    m_camera.pan(-50.0F, -30.0F);
    m_camera.setZoom(1.0F);
    return true;
}

void App::run() {
    m_renderer.render(m_model, m_camera);
}

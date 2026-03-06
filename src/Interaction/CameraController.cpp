#include "Interaction/CameraController.h"

#include <glm/vec2.hpp>

void CameraController::update(Camera2D& camera, const InputState& input) {
    if (input.middleDown) {
        if (!m_panning) {
            m_panning = true;
            m_lastMouseX = input.mouseX;
            m_lastMouseY = input.mouseY;
        } else {
            const double deltaX = input.mouseX - m_lastMouseX;
            const double deltaY = input.mouseY - m_lastMouseY;

            const glm::vec2 mouseDeltaWorld(
                static_cast<float>(deltaX) / camera.zoom(),
                static_cast<float>(-deltaY) / camera.zoom());

            camera.move(-mouseDeltaWorld);

            m_lastMouseX = input.mouseX;
            m_lastMouseY = input.mouseY;
        }
    } else {
        m_panning = false;
    }

    if (input.scrollDelta != 0.0f) {
        const float zoomStep = 0.1f;
        const float zoomFactor = 1.0f + (input.scrollDelta * zoomStep);
        camera.setZoom(camera.zoom() * zoomFactor);
    }
}

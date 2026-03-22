#pragma once

#include "Interaction/InputState.h"
#include "View/Camera2D.h"

class CameraController {
public:
	void update(Camera2D &camera, const InputState &input);

private:
	bool m_panning = false;
	double m_lastMouseX = 0.0;
	double m_lastMouseY = 0.0;
};

#pragma once

#include "Engine/DiagramModel.h"
#include "View/Camera2D.h"

namespace Render {

class EdgeRenderer {
public:
    void draw(const Engine::DiagramModel& model, const View::Camera2D& camera) const noexcept;
};

} // namespace Render

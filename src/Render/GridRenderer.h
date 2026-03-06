#pragma once

#include "View/Camera2D.h"

namespace Render {

class GridRenderer {
public:
    void draw(const View::Camera2D& camera) const noexcept;
};

} // namespace Render

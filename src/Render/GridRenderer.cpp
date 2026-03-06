#include "GridRenderer.h"

#include <iostream>

namespace Render {

void GridRenderer::draw(const View::Camera2D& camera) const noexcept {
    const auto& p = camera.position();
    std::cout << "[Render] grid at camera(" << p.x << "," << p.y << ") zoom=" << camera.zoom() << '\n';
}

} // namespace Render

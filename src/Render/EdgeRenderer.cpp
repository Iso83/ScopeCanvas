#include "EdgeRenderer.h"

#include <iostream>

namespace Render {

void EdgeRenderer::draw(const Engine::DiagramModel&, const View::Camera2D&) const noexcept {
    std::cout << "[Render] edges (none)\n";
}

} // namespace Render

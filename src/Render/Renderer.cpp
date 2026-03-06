#include "Renderer.h"

#include <iostream>

namespace Render {

void Renderer::render(const Engine::DiagramModel& model, const View::Camera2D& camera) {
    std::cout << "[Render] clear screen\n";
    m_gridRenderer.draw(camera);
    m_edgeRenderer.draw(model, camera);
    m_nodeRenderer.draw(model, camera);
}

} // namespace Render

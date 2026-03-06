#include "NodeRenderer.h"

#include <iostream>

namespace Render {

namespace {
constexpr float kBaseR = 0.18F;
constexpr float kBaseG = 0.23F;
constexpr float kBaseB = 0.30F;
constexpr float kSelectedR = 0.96F;
constexpr float kSelectedG = 0.64F;
constexpr float kSelectedB = 0.18F;
}

void NodeRenderer::draw(const Engine::DiagramModel& model, const View::Camera2D& camera) {
    rebuildVertexBuffer(model, camera);

    std::cout << "[Render] nodes count=" << model.nodes().size() << " batched_vertices=" << m_vbo.size() << '\n';
}

void NodeRenderer::rebuildVertexBuffer(const Engine::DiagramModel& model, const View::Camera2D& camera) {
    m_vbo.clear();
    m_vbo.reserve(model.nodes().size() * 6U);

    for (const Engine::Node& node : model.nodes()) {
        const Engine::Vec2 topLeft = camera.worldToView(node.position);
        const float w = node.size.x * camera.zoom();
        const float h = node.size.y * camera.zoom();

        const float r = node.selected ? kSelectedR : kBaseR;
        const float g = node.selected ? kSelectedG : kBaseG;
        const float b = node.selected ? kSelectedB : kBaseB;

        const NodeVertex v0 {topLeft.x, topLeft.y, r, g, b};
        const NodeVertex v1 {topLeft.x + w, topLeft.y, r, g, b};
        const NodeVertex v2 {topLeft.x + w, topLeft.y + h, r, g, b};
        const NodeVertex v3 {topLeft.x, topLeft.y + h, r, g, b};

        m_vbo.insert(m_vbo.end(), {v0, v1, v2, v0, v2, v3});
    }
}

} // namespace Render

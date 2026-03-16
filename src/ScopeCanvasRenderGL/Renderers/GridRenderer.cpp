#include "Renderers/GridRenderer.h"

namespace ScopeCanvas::RenderGL::Renderers
{
std::vector<Engine::Core::Vec2> GridRenderer::buildGridVertices(const Camera::Camera2D& camera,
                                                                float spacing,
                                                                int halfExtent) const
{
    std::vector<Engine::Core::Vec2> vertices;
    if (spacing <= 0.0F || halfExtent <= 0)
    {
        return vertices;
    }

    vertices.reserve(static_cast<std::size_t>(halfExtent * 8));
    for (int i = -halfExtent; i <= halfExtent; ++i)
    {
        const float axis = static_cast<float>(i) * spacing;
        vertices.push_back(camera.worldToScreen({axis, -static_cast<float>(halfExtent) * spacing}));
        vertices.push_back(camera.worldToScreen({axis, static_cast<float>(halfExtent) * spacing}));
        vertices.push_back(camera.worldToScreen({-static_cast<float>(halfExtent) * spacing, axis}));
        vertices.push_back(camera.worldToScreen({static_cast<float>(halfExtent) * spacing, axis}));
    }

    return vertices;
}
} // namespace ScopeCanvas::RenderGL::Renderers

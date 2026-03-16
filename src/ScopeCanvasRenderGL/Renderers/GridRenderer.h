#pragma once

#include <vector>

#include "Camera/Camera2D.h"
#include "ScopeCanvasEngineCore/Core/Vec2.h"

namespace ScopeCanvas::RenderGL::Renderers
{
class GridRenderer
{
public:
    [[nodiscard]] std::vector<Engine::Core::Vec2> buildGridVertices(const Camera::Camera2D& camera,
                                                                    float spacing,
                                                                    int halfExtent) const;
};
} // namespace ScopeCanvas::RenderGL::Renderers

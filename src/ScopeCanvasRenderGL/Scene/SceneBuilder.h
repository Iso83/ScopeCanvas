#pragma once

#include <vector>

#include "Scene/RenderScene.h"
#include "ScopeCanvasEngineCore/Core/DiagramModel.h"
#include "ScopeCanvasRouting/Routing/EdgeRoute.h"

namespace ScopeCanvas::RenderGL::Scene
{
class SceneBuilder
{
public:
    [[nodiscard]] RenderScene build(const Engine::Core::DiagramModel& model,
                                    const std::vector<Engine::Routing::EdgeRoute>& edgeRoutes) const;
};
} // namespace ScopeCanvas::RenderGL::Scene

#pragma once

#include <vector>

#include "ScopeCanvasEngineCore/Core/Vec2.h"
#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"

namespace ScopeCanvas::Engine::Routing
{
struct EdgeRoute
{
    Core::CanvasEdgeId edgeId{};
    std::vector<Core::Vec2> points{};
};
} // namespace ScopeCanvas::Engine::Routing

#pragma once

#include <vector>

#include "ScopeCanvasEngineCore/Core/Vec2.h"
#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"
#include "ScopeCanvasEngineCore/Ids/StrongId.h"

namespace ScopeCanvas::Engine::Routing
{
struct EdgeBundleIdTag;
using EdgeBundleId = Core::StrongId<EdgeBundleIdTag>;

struct EdgeBundle
{
    EdgeBundleId id{};
    Core::Vec2 position{};
    std::vector<Core::CanvasEdgeId> edges{};
};
} // namespace ScopeCanvas::Engine::Routing

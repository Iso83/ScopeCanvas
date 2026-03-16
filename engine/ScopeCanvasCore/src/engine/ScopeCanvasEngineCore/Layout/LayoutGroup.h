#pragma once

#include <vector>

#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"
#include "ScopeCanvasEngineCore/Ids/TypeIds.h"

namespace ScopeCanvas::Engine::Core
{
struct LayoutGroup
{
    LayoutGroupId id{};
    std::vector<CanvasNodeId> nodes{};
    bool preserveInternalLayout{};
};
} // namespace ScopeCanvas::Engine::Core

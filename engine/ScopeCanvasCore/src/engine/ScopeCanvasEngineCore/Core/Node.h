#pragma once

#include <vector>

#include "ScopeCanvasEngineCore/Core/Vec2.h"
#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"
#include "ScopeCanvasEngineCore/Ids/TypeIds.h"

namespace ScopeCanvas::Engine::Core
{
struct Node
{
    CanvasNodeId id{};
    NodeTypeId typeId{};
    Vec2 position{};
    Vec2 size{};
    CanvasNodeId parentId{};
    bool collapsed{};
    std::vector<LayoutGroupId> groups{};
    std::vector<CanvasConnectorId> connectors{};

    void setPosition(const Vec2& value)
    {
        position = value;
    }

    void setSize(const Vec2& value)
    {
        size = value;
    }
};
} // namespace ScopeCanvas::Engine::Core

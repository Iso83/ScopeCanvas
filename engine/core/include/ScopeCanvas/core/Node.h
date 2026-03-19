#pragma once

#include <ScopeCanvas/core/Vec2.h>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/TypeIds.h>
#include <vector>

namespace ScopeCanvas::Core {
struct Node {
    CanvasNodeId id{};
    NodeTypeId typeId{};
    Vec2 position{};
    Vec2 size{};
    CanvasNodeId parentId{};
    bool collapsed{};
    std::vector<LayoutGroupId> groups{};
    std::vector<CanvasConnectorId> connectors{};

    void setPosition(const Vec2& value) {
        position = value;
    }

    void setSize(const Vec2& value) {
        size = value;
    }
};
} // namespace ScopeCanvas::Core

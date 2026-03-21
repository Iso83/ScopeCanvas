#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/TypeIds.h>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Core {
struct Node {
    CanvasNodeId id{};
    NodeTypeId typeId{};
    glm::vec2 position{};
    glm::vec2 size{};
    CanvasNodeId parentId{};
    bool collapsed{};
    std::vector<LayoutGroupId> groups{};
    std::vector<CanvasConnectorId> connectors{};

    void setPosition(const glm::vec2& value) {
        position = value;
    }

    void setSize(const glm::vec2& value) {
        size = value;
    }
};
} // namespace ScopeCanvas::Core

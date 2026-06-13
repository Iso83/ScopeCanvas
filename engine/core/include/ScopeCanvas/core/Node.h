#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/TypeIds.h>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Core {
struct Node {
    Ids::NodeId id{};
    Ids::NodeTypeId typeId{};
    glm::vec2 position{};
    glm::vec2 size{};
    std::vector<Ids::ConnectorId> connectors{};
};
} // namespace ScopeCanvas::Core

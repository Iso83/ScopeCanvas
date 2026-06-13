#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/TypeIds.h>
#include <cstdint>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Render::Scene {
struct NodeRenderData {
    Core::Ids::NodeId id{};
    Core::Ids::NodeTypeId typeId{};
    glm::vec2 position{};
    glm::vec2 size{};
    std::uint32_t connectorCount{};
};

struct ConnectorAnchorRenderData {
    Core::Ids::ConnectorId connectorId{};
    Core::Ids::NodeId nodeId{};
    glm::vec2 anchor{};
    bool output{};
};

struct EdgeRenderData {
    Core::Ids::EdgeId edgeId{};
    std::vector<glm::vec2> points{};
};

struct RenderScene {
    std::vector<NodeRenderData> nodes{};
    std::vector<ConnectorAnchorRenderData> connectorAnchors{};
    std::vector<EdgeRenderData> edges{};
};
} // namespace ScopeCanvas::Render::Scene

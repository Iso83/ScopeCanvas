#pragma once

#include <ScopeCanvas/core/flow/FlowDocument.h>
#include <glm/vec2.hpp>

namespace ScopeCanvas::Routing::Flow {

struct FlowLayoutOptions {
    glm::vec2 origin{96.0F, 180.0F};
    glm::vec2 stepSize{210.0F, 82.0F};
    float stepGap{92.0F};
    float childIndent{54.0F};
    float childGap{34.0F};
    float groupHeaderHeight{42.0F};
    float groupGap{84.0F};
    float rowGap{40.0F};
    float headerToRailGap{116.0F};
    float parentPadding{24.0F};
    float rowStartPadding{24.0F};
    float railOuterPadding{64.0F};
    float minGroupContentWidth{640.0F};
    float minGroupHeaderWidth{720.0F};
    float collapsedGroupWidth{320.0F};
};

struct FlowStepLayout {
    Core::Ids::NodeId stepId{};
    Core::Flow::FlowRowId rowId{};
    Core::Ids::NodeId parentStepId{};
    glm::vec2 position{};
    glm::vec2 size{};
    glm::vec2 subtreePosition{};
    glm::vec2 subtreeSize{};
    glm::vec2 railAnchor{};
    bool hasChildren{};
    bool collapsed{};
    std::uint32_t depth{};
};

struct FlowRowLayout {
    Core::Flow::FlowRowId rowId{};
    glm::vec2 railStart{};
    glm::vec2 railEnd{};
    float top{};
    float bottom{};
};

struct FlowGroupLayout {
    Core::Flow::FlowGroupId groupId{};
    glm::vec2 headerPosition{};
    glm::vec2 headerSize{};
    glm::vec2 boundsPosition{};
    glm::vec2 boundsSize{};
    float bottom{};
};

struct FlowLayoutResult {
    std::vector<FlowGroupLayout> groups{};
    std::vector<FlowRowLayout> rows{};
    std::vector<FlowStepLayout> steps{};

    const FlowStepLayout* step(Core::Ids::NodeId stepId) const;
};

class FlowLayout {
  public:
    [[nodiscard]] FlowLayoutResult build(const Core::Flow::FlowDocument& document,
                                         const FlowLayoutOptions& options = {}) const;
    [[nodiscard]] std::size_t insertionIndex(const FlowLayoutResult& layout, Core::Flow::FlowRowId rowId,
                                             float worldX) const;
};

} // namespace ScopeCanvas::Routing::Flow

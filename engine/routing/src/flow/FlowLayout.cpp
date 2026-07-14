#include <ScopeCanvas/routing/flow/FlowLayout.h>
#include <algorithm>

namespace ScopeCanvas::Routing::Flow {
namespace {
using ScopeCanvas::Core::Flow::FlowGroup;
using ScopeCanvas::Core::Flow::FlowRow;
using ScopeCanvas::Core::Flow::FlowStep;

struct SubtreeResult {
    glm::vec2 position{};
    glm::vec2 size{};
    glm::vec2 anchor{};
};

SubtreeResult layoutStep(const FlowStep& step, Core::Flow::FlowRowId rowId, Core::Ids::NodeId parentStepId,
                         glm::vec2 position, std::uint32_t depth, const FlowLayoutOptions& options,
                         FlowLayoutResult& result) {
    FlowStepLayout layout{};
    layout.stepId = step.id;
    layout.rowId = rowId;
    layout.parentStepId = parentStepId;
    layout.position = position;
    layout.size = options.stepSize;
    layout.railAnchor = {position.x + options.stepSize.x * 0.5F, position.y + options.stepSize.y * 0.5F};
    layout.hasChildren = !step.children.empty();
    layout.collapsed = step.collapsed;
    layout.depth = depth;

    float minX = position.x;
    float maxX = position.x + options.stepSize.x;
    float minY = position.y;
    float maxY = position.y + options.stepSize.y;

    if (!step.collapsed && !step.children.empty()) {
        float childX = position.x + options.childIndent;
        float childY = position.y - options.stepSize.y - options.childGap;
        for (const FlowStep& child : step.children) {
            const SubtreeResult childResult = layoutStep(child, rowId, step.id, {childX, childY}, depth + 1U, options, result);
            childX += childResult.size.x + options.stepGap;
            minX = std::min(minX, childResult.position.x);
            maxX = std::max(maxX, childResult.position.x + childResult.size.x);
            minY = std::min(minY, childResult.position.y);
            maxY = std::max(maxY, childResult.position.y + childResult.size.y);
        }
    }

    layout.subtreePosition = {minX - options.parentPadding, minY - options.parentPadding};
    layout.subtreeSize = {maxX - minX + options.parentPadding * 2.0F, maxY - minY + options.parentPadding * 2.0F};
    result.steps.push_back(layout);
    return {layout.subtreePosition, layout.subtreeSize, layout.railAnchor};
}
} // namespace

const FlowStepLayout* FlowLayoutResult::step(Core::Ids::NodeId stepId) const {
    const auto it = std::find_if(steps.begin(), steps.end(), [stepId](const FlowStepLayout& item) {
        return item.stepId == stepId;
    });
    return it == steps.end() ? nullptr : &(*it);
}

FlowLayoutResult FlowLayout::build(const Core::Flow::FlowDocument& document, const FlowLayoutOptions& options) const {
    FlowLayoutResult result{};
    float groupHeaderY = options.origin.y;

    for (const FlowGroup& group : document.groups()) {
        const float rowRailY = groupHeaderY - options.headerToRailGap;
        float contentLeft = options.origin.x;
        float contentRight = options.origin.x + options.minGroupContentWidth;
        float contentTop = rowRailY + options.stepSize.y * 0.5F;
        float contentBottom = rowRailY - options.stepSize.y * 0.5F;
        float rowY = rowRailY;

        if (!group.collapsed) {
            for (const FlowRow& row : group.rows) {
                float stepX = options.origin.x + options.rowStartPadding;
                std::vector<glm::vec2> anchors;
                float rowTop = rowY + options.stepSize.y * 0.5F;
                float rowBottom = rowY - options.stepSize.y * 0.5F;

                for (const FlowStep& step : row.steps) {
                    const SubtreeResult subtree =
                        layoutStep(step, row.id, {}, {stepX, rowY - options.stepSize.y * 0.5F}, 0U, options, result);
                    anchors.push_back(subtree.anchor);
                    stepX += subtree.size.x + options.stepGap;
                    contentLeft = std::min(contentLeft, subtree.position.x);
                    contentRight = std::max(contentRight, subtree.position.x + subtree.size.x);
                    rowTop = std::max(rowTop, subtree.position.y + subtree.size.y);
                    rowBottom = std::min(rowBottom, subtree.position.y);
                }

                const float railStartX =
                    anchors.empty() ? options.origin.x : anchors.front().x - options.stepSize.x * 0.5F - options.railOuterPadding;
                const float railEndX =
                    anchors.empty() ? options.origin.x + options.stepSize.x : anchors.back().x + options.stepSize.x * 0.5F + options.railOuterPadding;
                result.rows.push_back({row.id, {railStartX, rowY}, {railEndX, rowY}, rowTop, rowBottom});
                contentLeft = std::min(contentLeft, railStartX);
                contentRight = std::max(contentRight, railEndX);
                contentTop = std::max(contentTop, rowTop);
                contentBottom = std::min(contentBottom, rowBottom);
                rowY = rowBottom - options.rowGap - options.stepSize.y * 0.5F;
            }
        }

        const glm::vec2 headerPos{options.origin.x, groupHeaderY};
        const glm::vec2 headerSize{std::max(options.minGroupHeaderWidth, contentRight - options.origin.x),
                                   options.groupHeaderHeight};
        if (group.collapsed) {
            contentLeft = headerPos.x;
            contentRight = headerPos.x + options.collapsedGroupWidth;
            contentBottom = headerPos.y;
        }
        result.groups.push_back({group.id, headerPos, headerSize, {contentLeft, contentBottom},
                                 {contentRight - contentLeft, groupHeaderY + options.groupHeaderHeight - contentBottom},
                                 contentBottom});
        groupHeaderY = contentBottom - options.groupGap;
    }

    return result;
}

std::size_t FlowLayout::insertionIndex(const FlowLayoutResult& layout, Core::Flow::FlowRowId rowId, float worldX) const {
    std::size_t index = 0;
    for (const FlowStepLayout& step : layout.steps) {
        if (step.rowId != rowId || step.depth != 0U)
            continue;
        if (worldX < step.railAnchor.x)
            return index;
        ++index;
    }
    return index;
}

} // namespace ScopeCanvas::Routing::Flow

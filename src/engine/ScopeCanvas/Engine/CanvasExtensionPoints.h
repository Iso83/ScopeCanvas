#pragma once

#include "Engine/DiagramModel.h"

#include <optional>
#include <string>
#include <vector>

struct CanvasNodeBadge {
    std::string text;
    uint32_t colorRgba = 0xFFFFFFFFu;
};

struct CanvasNodeStyle {
    uint32_t bodyColorRgba = 0x2E323BFFu;
    uint32_t titleColorRgba = 0x1B1F26FFu;
    uint32_t borderColorRgba = 0xD8D8E0FFu;
};

struct CanvasNodePresentation {
    CanvasNodeStyle style;
    std::vector<CanvasNodeBadge> badges;
};

class INodePresentationProvider {
public:
    virtual ~INodePresentationProvider() = default;
    virtual std::optional<CanvasNodePresentation> presentationFor(const Node &node) const = 0;
};

class IChildLayoutPolicy {
public:
    virtual ~IChildLayoutPolicy() = default;
    virtual void apply(DiagramModel &model, CanvasNodeId parentId) const = 0;
};

#include <ScopeCanvas/widget/theme/NodeVisualRegistry.h>

namespace ScopeCanvas::Render::Theme {
namespace {
NodeVisual makeVisual(const char* title, const char* icon, ColorRgba8 titleBar, ColorRgba8 body,
                      ColorRgba8 outputConnector) {
    NodeVisual visual{};
    visual.title = title;
    visual.icon = icon;
    visual.titleBarColor = titleBar;
    visual.bodyColor = body;
    visual.connectorOutputColor = outputConnector;
    visual.borderColor = {235, 235, 240, 180};
    visual.titleTextColor = {245, 245, 245, 255};
    visual.connectorInputColor = {215, 215, 230, 255};
    visual.selectionColor = {255, 210, 120, 220};
    visual.cornerRadius = 6.0F;
    visual.borderThickness = 1.0F;
    visual.titleBarHeight = 24.0F;
    return visual;
}
} // namespace

NodeVisualRegistry::NodeVisualRegistry() {
    m_defaultVisual = makeVisual("Node", "N", {28, 32, 40, 220}, {64, 68, 80, 210}, {225, 225, 240, 255});

    registerVisual(Core::NodeTypeId{1},
                   makeVisual("Number", "1", {30, 34, 44, 220}, {70, 74, 86, 210}, {225, 225, 240, 255}));
    registerVisual(Core::NodeTypeId{2},
                   makeVisual("Add", "+", {32, 38, 52, 220}, {80, 90, 118, 210}, {225, 225, 240, 255}));
    registerVisual(Core::NodeTypeId{3},
                   makeVisual("Multiply", "*", {44, 34, 26, 220}, {120, 90, 54, 210}, {255, 200, 120, 255}));
    registerVisual(Core::NodeTypeId{4},
                   makeVisual("Output", "O", {24, 44, 34, 220}, {50, 104, 78, 210}, {120, 255, 170, 255}));

    registerVisual(Core::NodeTypeId{10},
                   makeVisual("Bits Container", "[]", {24, 28, 38, 230}, {52, 58, 74, 220}, {225, 225, 240, 255}));
    registerVisual(Core::NodeTypeId{11},
                   makeVisual("Bit", "b", {62, 56, 18, 230}, {128, 118, 36, 220}, {255, 228, 120, 255}));
    registerVisual(Core::NodeTypeId{12},
                   makeVisual("Loop", "L", {26, 40, 68, 230}, {50, 96, 162, 220}, {120, 190, 255, 255}));
    registerVisual(Core::NodeTypeId{13},
                   makeVisual("Const True", "T", {18, 58, 30, 230}, {42, 126, 68, 220}, {110, 255, 146, 255}));
    registerVisual(Core::NodeTypeId{14},
                   makeVisual("Const False", "F", {68, 24, 24, 230}, {138, 48, 48, 220}, {255, 120, 120, 255}));
    registerVisual(Core::NodeTypeId{15},
                   makeVisual("BitShift", ">>", {66, 40, 18, 230}, {138, 88, 44, 220}, {255, 188, 110, 255}));
}

void NodeVisualRegistry::registerVisual(Core::NodeTypeId typeId, const NodeVisual& visual) {
    m_visuals[typeId.value()] = visual;
}

const NodeVisual& NodeVisualRegistry::getVisual(Core::NodeTypeId typeId) const {
    const auto it = m_visuals.find(typeId.value());
    return it == m_visuals.end() ? m_defaultVisual : it->second;
}
} // namespace ScopeCanvas::Render::Theme

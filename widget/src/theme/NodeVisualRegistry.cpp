#include <ScopeCanvas/widget/theme/NodeVisualRegistry.h>

namespace ScopeCanvas::Render::Theme {
namespace {
NodeVisual makeVisual(const char* title, const char* icon, ColorRgba8 titleBar, ColorRgba8 body, ColorRgba8 border,
                      ColorRgba8 outputConnector) {
    NodeVisual visual{};
    visual.title = title;
    visual.icon = icon;
    visual.titleBarColor = titleBar;
    visual.bodyColor = body;
    visual.borderColor = border;
    visual.connectorOutputColor = outputConnector;
    visual.titleTextColor = {242, 244, 248, 255};
    visual.connectorInputColor = {210, 216, 232, 255};
    visual.selectionColor = {255, 196, 102, 228};
    visual.cornerRadius = 6.0F;
    visual.borderThickness = 1.25F;
    visual.titleBarHeight = 24.0F;
    return visual;
}
} // namespace

NodeVisualRegistry::NodeVisualRegistry() {
    m_defaultVisual = makeVisual("Node", "N", {20, 24, 31, 235}, {30, 35, 43, 226}, {68, 78, 96, 255},
                                 {208, 216, 236, 255});

    registerVisual(Core::NodeTypeId{1},
                   makeVisual("Number", "1", {24, 28, 36, 235}, {38, 43, 54, 226}, {83, 93, 112, 255},
                              {214, 220, 238, 255}));
    registerVisual(Core::NodeTypeId{2},
                   makeVisual("Add", "+", {18, 30, 44, 235}, {28, 47, 69, 226}, {66, 118, 176, 255},
                              {118, 194, 255, 255}));
    registerVisual(Core::NodeTypeId{3},
                   makeVisual("Multiply", "*", {45, 28, 18, 235}, {68, 41, 27, 226}, {161, 102, 58, 255},
                              {255, 186, 112, 255}));
    registerVisual(Core::NodeTypeId{4},
                   makeVisual("Output", "O", {17, 38, 30, 235}, {26, 57, 44, 226}, {68, 154, 115, 255},
                              {124, 245, 174, 255}));

    registerVisual(Core::NodeTypeId{10},
                   makeVisual("Message Block", "[]", {19, 24, 34, 238}, {27, 33, 46, 228}, {88, 108, 148, 255},
                              {204, 214, 236, 255}));
    registerVisual(Core::NodeTypeId{11},
                   makeVisual("Round Const", "b", {44, 35, 14, 238}, {61, 50, 21, 228}, {170, 140, 57, 255},
                              {255, 214, 101, 255}));
    registerVisual(Core::NodeTypeId{12},
                   makeVisual("Loop", "L", {18, 33, 55, 238}, {24, 48, 84, 228}, {70, 138, 214, 255},
                              {110, 190, 255, 255}));
    registerVisual(Core::NodeTypeId{13},
                   makeVisual("Choose", "T", {17, 42, 31, 238}, {26, 61, 43, 228}, {70, 163, 112, 255},
                              {118, 245, 168, 255}));
    registerVisual(Core::NodeTypeId{14},
                   makeVisual("Mix", "F", {53, 20, 28, 238}, {78, 28, 39, 228}, {186, 70, 96, 255},
                              {255, 124, 140, 255}));
    registerVisual(Core::NodeTypeId{15},
                   makeVisual("Sigma", ">>", {54, 31, 13, 238}, {78, 45, 20, 228}, {186, 116, 57, 255},
                              {255, 190, 110, 255}));
}

void NodeVisualRegistry::registerVisual(Core::NodeTypeId typeId, const NodeVisual& visual) {
    m_visuals[typeId.value()] = visual;
}

const NodeVisual& NodeVisualRegistry::getVisual(Core::NodeTypeId typeId) const {
    const auto it = m_visuals.find(typeId.value());
    return it == m_visuals.end() ? m_defaultVisual : it->second;
}
} // namespace ScopeCanvas::Render::Theme

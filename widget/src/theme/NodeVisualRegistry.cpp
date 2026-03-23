#include <ScopeCanvas/widget/theme/NodeVisualRegistry.h>

namespace ScopeCanvas::Render::Theme {
namespace {
NodeVisual makeVisual(const char* title, const char* icon, ColorRgba8 titleBar, ColorRgba8 titleBarAccent,
                      ColorRgba8 body, ColorRgba8 border, ColorRgba8 outputConnector, ColorRgba8 iconColor) {
    NodeVisual visual{};
    visual.title = title;
    visual.icon = icon;
    visual.titleBarColor = titleBar;
    visual.titleBarAccentColor = titleBarAccent;
    visual.bodyColor = body;
    visual.borderColor = border;
    visual.connectorOutputColor = outputConnector;
    visual.titleTextColor = {242, 244, 248, 255};
    visual.iconColor = iconColor;
    visual.connectorInputColor = {210, 216, 232, 255};
    visual.selectionColor = {255, 196, 102, 228};
    visual.cornerRadius = 12.0F;
    visual.borderThickness = 1.25F;
    visual.titleBarHeight = 28.0F;
    return visual;
}
} // namespace

NodeVisualRegistry::NodeVisualRegistry() {
    m_defaultVisual = makeVisual("Node", "N", {18, 22, 28, 242}, {56, 79, 116, 222}, {24, 29, 37, 232},
                                 {68, 78, 96, 255}, {208, 216, 236, 255}, {242, 245, 252, 255});

    registerVisual(Core::NodeTypeId{1},
                   makeVisual("Number", "1", {24, 28, 36, 242}, {92, 112, 148, 222}, {38, 43, 54, 228},
                              {83, 93, 112, 255}, {214, 220, 238, 255}, {235, 239, 248, 255}));
    registerVisual(Core::NodeTypeId{2},
                   makeVisual("Add", "+", {18, 30, 44, 242}, {66, 124, 196, 226}, {28, 47, 69, 228},
                              {66, 118, 176, 255}, {118, 194, 255, 255}, {222, 242, 255, 255}));
    registerVisual(Core::NodeTypeId{3},
                   makeVisual("Multiply", "*", {45, 28, 18, 242}, {178, 96, 46, 226}, {68, 41, 27, 228},
                              {161, 102, 58, 255}, {255, 186, 112, 255}, {255, 230, 204, 255}));
    registerVisual(Core::NodeTypeId{4},
                   makeVisual("Output", "O", {17, 38, 30, 242}, {56, 126, 92, 226}, {26, 57, 44, 228},
                              {68, 154, 115, 255}, {124, 245, 174, 255}, {224, 255, 236, 255}));

    registerVisual(Core::NodeTypeId{10},
                   makeVisual("Message Block", "[]", {19, 24, 34, 242}, {82, 106, 158, 224}, {27, 33, 46, 230},
                              {88, 108, 148, 255}, {204, 214, 236, 255}, {230, 236, 250, 255}));
    registerVisual(Core::NodeTypeId{11},
                   makeVisual("Round Const", "b", {44, 35, 14, 242}, {164, 128, 48, 224}, {61, 50, 21, 230},
                              {170, 140, 57, 255}, {255, 214, 101, 255}, {255, 241, 199, 255}));
    registerVisual(Core::NodeTypeId{12},
                   makeVisual("Loop", "L", {18, 33, 55, 242}, {70, 138, 214, 224}, {24, 48, 84, 230},
                              {70, 138, 214, 255}, {110, 190, 255, 255}, {230, 245, 255, 255}));
    registerVisual(Core::NodeTypeId{13},
                   makeVisual("Choose", "T", {17, 42, 31, 242}, {66, 156, 110, 224}, {26, 61, 43, 230},
                              {70, 163, 112, 255}, {118, 245, 168, 255}, {232, 255, 238, 255}));
    registerVisual(Core::NodeTypeId{14},
                   makeVisual("Mix", "F", {53, 20, 28, 242}, {182, 72, 98, 224}, {78, 28, 39, 230}, {186, 70, 96, 255},
                              {255, 124, 140, 255}, {255, 229, 234, 255}));
    registerVisual(Core::NodeTypeId{15},
                   makeVisual("Sigma", ">>", {54, 31, 13, 242}, {196, 118, 56, 224}, {78, 45, 20, 230},
                              {186, 116, 57, 255}, {255, 190, 110, 255}, {255, 235, 204, 255}));
}

void NodeVisualRegistry::registerVisual(Core::NodeTypeId typeId, const NodeVisual& visual) {
    m_visuals[typeId.value()] = visual;
}

const NodeVisual& NodeVisualRegistry::getVisual(Core::NodeTypeId typeId) const {
    const auto it = m_visuals.find(typeId.value());
    return it == m_visuals.end() ? m_defaultVisual : it->second;
}
} // namespace ScopeCanvas::Render::Theme

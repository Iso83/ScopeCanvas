#include <ScopeCanvas/widget/theme/NodeVisualRegistry.h>

namespace ScopeCanvas::Render::Theme {
namespace {
glm::vec4 rgba(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255) {
    return {static_cast<float>(r) / 255.0F, static_cast<float>(g) / 255.0F, static_cast<float>(b) / 255.0F,
            static_cast<float>(a) / 255.0F};
}

NodeVisual makeVisual(const char* title, const char* icon, NodeIconShape iconShape, glm::vec4 titleBar,
                      glm::vec4 titleBarAccent, glm::vec4 body, glm::vec4 border, glm::vec4 outputConnector,
                      glm::vec4 iconColor) {
    NodeVisual visual{};
    visual.title = title;
    visual.icon = icon;
    visual.iconShape = iconShape;
    visual.headerColor = titleBar;
    visual.headerAccentColor = titleBarAccent;
    visual.style.bodyColor = body;
    visual.style.borderColor = border;
    visual.connectorOutputColor = outputConnector;
    visual.textColor = rgba(242, 244, 248, 255);
    visual.iconColor = iconColor;
    visual.connectorInputColor = rgba(210, 216, 232, 255);
    visual.style.selectionColor = rgba(255, 196, 102, 228);
    visual.style.cornerRadius = 12.0F;
    visual.style.borderThickness = 1.25F;
    visual.headerHeight = 28.0F;
    return visual;
}
} // namespace

NodeVisualRegistry::NodeVisualRegistry() {
    m_defaultVisual =
        makeVisual("Node", "N", NodeIconShape::Circle, rgba(18, 22, 28, 242), rgba(56, 79, 116, 222),
                   rgba(24, 29, 37, 232), rgba(68, 78, 96, 255), rgba(208, 216, 236, 255), rgba(242, 245, 252, 255));

    registerVisual(Core::Ids::NodeTypeId{1},
                   makeVisual("Number", "1", NodeIconShape::Circle, rgba(24, 28, 36, 242), rgba(92, 112, 148, 222),
                              rgba(38, 43, 54, 228), rgba(83, 93, 112, 255), rgba(214, 220, 238, 255),
                              rgba(235, 239, 248, 255)));
    registerVisual(Core::Ids::NodeTypeId{2},
                   makeVisual("Add", "+", NodeIconShape::Diamond, rgba(18, 30, 44, 242), rgba(66, 124, 196, 226),
                              rgba(28, 47, 69, 228), rgba(66, 118, 176, 255), rgba(118, 194, 255, 255),
                              rgba(222, 242, 255, 255)));
    registerVisual(Core::Ids::NodeTypeId{3},
                   makeVisual("Multiply", "*", NodeIconShape::Triangle, rgba(45, 28, 18, 242), rgba(178, 96, 46, 226),
                              rgba(68, 41, 27, 228), rgba(161, 102, 58, 255), rgba(255, 186, 112, 255),
                              rgba(255, 230, 204, 255)));
    registerVisual(Core::Ids::NodeTypeId{4},
                   makeVisual("Output", "O", NodeIconShape::Square, rgba(17, 38, 30, 242), rgba(56, 126, 92, 226),
                              rgba(26, 57, 44, 228), rgba(68, 154, 115, 255), rgba(124, 245, 174, 255),
                              rgba(224, 255, 236, 255)));

    registerVisual(Core::Ids::NodeTypeId{10},
                   makeVisual("Message Block", "[]", NodeIconShape::Square, rgba(19, 24, 34, 242),
                              rgba(82, 106, 158, 224), rgba(27, 33, 46, 230), rgba(88, 108, 148, 255),
                              rgba(204, 214, 236, 255), rgba(230, 236, 250, 255)));
    registerVisual(Core::Ids::NodeTypeId{11},
                   makeVisual("Round Const", "b", NodeIconShape::Circle, rgba(44, 35, 14, 242), rgba(164, 128, 48, 224),
                              rgba(61, 50, 21, 230), rgba(170, 140, 57, 255), rgba(255, 214, 101, 255),
                              rgba(255, 241, 199, 255)));
    registerVisual(Core::Ids::NodeTypeId{12},
                   makeVisual("Loop", "L", NodeIconShape::Triangle, rgba(18, 33, 55, 242), rgba(70, 138, 214, 224),
                              rgba(24, 48, 84, 230), rgba(70, 138, 214, 255), rgba(110, 190, 255, 255),
                              rgba(230, 245, 255, 255)));
    registerVisual(Core::Ids::NodeTypeId{13},
                   makeVisual("Choose", "T", NodeIconShape::Diamond, rgba(17, 42, 31, 242), rgba(66, 156, 110, 224),
                              rgba(26, 61, 43, 230), rgba(70, 163, 112, 255), rgba(118, 245, 168, 255),
                              rgba(232, 255, 238, 255)));
    registerVisual(Core::Ids::NodeTypeId{14},
                   makeVisual("Mix", "F", NodeIconShape::Diamond, rgba(53, 20, 28, 242), rgba(182, 72, 98, 224),
                              rgba(78, 28, 39, 230), rgba(186, 70, 96, 255), rgba(255, 124, 140, 255),
                              rgba(255, 229, 234, 255)));
    registerVisual(Core::Ids::NodeTypeId{15},
                   makeVisual("Sigma", ">>", NodeIconShape::Chevrons, rgba(54, 31, 13, 242), rgba(196, 118, 56, 224),
                              rgba(78, 45, 20, 230), rgba(186, 116, 57, 255), rgba(255, 190, 110, 255),
                              rgba(255, 235, 204, 255)));
}

void NodeVisualRegistry::registerVisual(Core::Ids::NodeTypeId typeId, const NodeVisual& visual) {
    m_visuals[typeId.value()] = visual;
}

const NodeVisual& NodeVisualRegistry::getVisual(Core::Ids::NodeTypeId typeId) const {
    const auto it = m_visuals.find(typeId.value());
    return it == m_visuals.end() ? m_defaultVisual : it->second;
}
} // namespace ScopeCanvas::Render::Theme

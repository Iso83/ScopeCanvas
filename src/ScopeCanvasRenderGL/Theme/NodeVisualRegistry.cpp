#include "Theme/NodeVisualRegistry.h"

namespace ScopeCanvas::Render::Theme
{
namespace
{
NodeVisual makeVisual(const char* title,
                      const char* icon,
                      ColorRgba8 body,
                      ColorRgba8 outputConnector)
{
    NodeVisual visual{};
    visual.title = title;
    visual.icon = icon;
    visual.bodyColor = body;
    visual.connectorOutputColor = outputConnector;
    return visual;
}
} // namespace

NodeVisualRegistry::NodeVisualRegistry()
{
    m_defaultVisual = makeVisual("Node", "N", {130, 130, 130, 95}, {225, 225, 240, 255});

    registerVisual(Engine::Core::NodeTypeId{1}, makeVisual("Number", "1", {130, 130, 130, 95}, {225, 225, 240, 255}));
    registerVisual(Engine::Core::NodeTypeId{2}, makeVisual("Add", "+", {130, 130, 130, 95}, {225, 225, 240, 255}));
    registerVisual(Engine::Core::NodeTypeId{3}, makeVisual("Multiply", "*", {130, 130, 130, 95}, {225, 225, 240, 255}));
    registerVisual(Engine::Core::NodeTypeId{4}, makeVisual("Output", "O", {130, 130, 130, 95}, {225, 225, 240, 255}));

    registerVisual(Engine::Core::NodeTypeId{10}, makeVisual("Bits Container", "[]", {70, 74, 82, 170}, {225, 225, 240, 255}));
    registerVisual(Engine::Core::NodeTypeId{11}, makeVisual("Bit", "b", {200, 200, 120, 120}, {225, 225, 240, 255}));
    registerVisual(Engine::Core::NodeTypeId{12}, makeVisual("Loop", "L", {80, 120, 190, 120}, {225, 225, 240, 255}));
    registerVisual(Engine::Core::NodeTypeId{13}, makeVisual("Const True", "T", {70, 180, 90, 140}, {70, 240, 90, 255}));
    registerVisual(Engine::Core::NodeTypeId{14}, makeVisual("Const False", "F", {180, 80, 80, 140}, {255, 90, 90, 255}));
    registerVisual(Engine::Core::NodeTypeId{15}, makeVisual("BitShift", ">>", {170, 120, 70, 130}, {255, 180, 90, 255}));
}

void NodeVisualRegistry::registerVisual(Engine::Core::NodeTypeId typeId, const NodeVisual& visual)
{
    m_visuals[typeId.value()] = visual;
}

const NodeVisual& NodeVisualRegistry::getVisual(Engine::Core::NodeTypeId typeId) const
{
    const auto it = m_visuals.find(typeId.value());
    return it == m_visuals.end() ? m_defaultVisual : it->second;
}
} // namespace ScopeCanvas::Render::Theme

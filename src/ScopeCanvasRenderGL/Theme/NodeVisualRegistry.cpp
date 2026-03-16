#include "Theme/NodeVisualRegistry.h"

namespace ScopeCanvas::RenderGL::Theme
{
void NodeVisualRegistry::registerVisual(Engine::Core::NodeTypeId typeId, const NodeVisual& visual)
{
    m_visuals[typeId.value()] = visual;
}

const NodeVisual& NodeVisualRegistry::getVisual(Engine::Core::NodeTypeId typeId) const
{
    const auto it = m_visuals.find(typeId.value());
    return it == m_visuals.end() ? m_defaultVisual : it->second;
}
} // namespace ScopeCanvas::RenderGL::Theme

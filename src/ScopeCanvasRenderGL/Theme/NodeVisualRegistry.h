#pragma once

#include <cstdint>
#include <unordered_map>

#include "ScopeCanvasEngineCore/Ids/TypeIds.h"

namespace ScopeCanvas::RenderGL::Theme
{
struct NodeVisual
{
    float cornerRadius{8.0F};
    float borderThickness{1.0F};
};

class NodeVisualRegistry
{
public:
    void registerVisual(Engine::Core::NodeTypeId typeId, const NodeVisual& visual);
    [[nodiscard]] const NodeVisual& getVisual(Engine::Core::NodeTypeId typeId) const;

private:
    std::unordered_map<std::uint32_t, NodeVisual> m_visuals{};
    NodeVisual m_defaultVisual{};
};
} // namespace ScopeCanvas::RenderGL::Theme

#pragma once

#include <ScopeCanvas/core/ids/TypeIds.h>
#include <ScopeCanvas/render/NodeRenderer.h>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace ScopeCanvas::Widget::Render::Theme {
enum class NodeIconShape {
    Circle,
    Square,
    Triangle,
    Diamond,
    Chevrons,
};

struct NodeVisual {
    std::string title{};
    std::string subtitle{};
    std::string icon{};
    NodeIconShape iconShape{NodeIconShape::Circle};
    ScopeCanvas::Render::NodeRenderStyle style{};
    glm::vec4 headerColor{0.12F, 0.14F, 0.18F, 0.95F};
    glm::vec4 headerAccentColor{0.28F, 0.38F, 0.52F, 0.95F};
    glm::vec4 textColor{0.95F, 0.96F, 0.98F, 1.0F};
    glm::vec4 iconColor{0.95F, 0.96F, 0.98F, 1.0F};
    glm::vec4 connectorInputColor{0.84F, 0.86F, 0.92F, 1.0F};
    glm::vec4 connectorOutputColor{0.88F, 0.88F, 0.94F, 1.0F};
    float headerHeight{28.0F};
};

class NodeVisualRegistry {
  private:
    std::unordered_map<std::uint32_t, NodeVisual> m_visuals{};
    NodeVisual m_defaultVisual{};

  public:
    NodeVisualRegistry();

    void registerVisual(Core::Ids::NodeTypeId typeId, const NodeVisual& visual);
    [[nodiscard]] const NodeVisual& getVisual(Core::Ids::NodeTypeId typeId) const;
};
} // namespace ScopeCanvas::Widget::Render::Theme

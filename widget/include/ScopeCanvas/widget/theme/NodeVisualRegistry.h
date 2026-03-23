#pragma once

#include <ScopeCanvas/core/ids/TypeIds.h>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace ScopeCanvas::Render::Theme {
struct ColorRgba8 {
    std::uint8_t r{0};
    std::uint8_t g{0};
    std::uint8_t b{0};
    std::uint8_t a{255};
};

struct NodeVisual {
    std::string title{};
    std::string icon{};

    ColorRgba8 titleBarColor{30, 34, 44, 185};
    ColorRgba8 titleBarAccentColor{58, 78, 110, 215};
    ColorRgba8 bodyColor{130, 130, 130, 95};
    ColorRgba8 borderColor{235, 235, 240, 180};
    ColorRgba8 titleTextColor{245, 245, 245, 255};
    ColorRgba8 iconColor{245, 245, 245, 255};
    ColorRgba8 connectorInputColor{215, 215, 230, 255};
    ColorRgba8 connectorOutputColor{225, 225, 240, 255};
    ColorRgba8 selectionColor{255, 210, 120, 220};

    float cornerRadius{4.0F};
    float borderThickness{1.0F};
    float titleBarHeight{22.0F};
};

class NodeVisualRegistry {
  public:
    NodeVisualRegistry();

    void registerVisual(Core::NodeTypeId typeId, const NodeVisual& visual);
    [[nodiscard]] const NodeVisual& getVisual(Core::NodeTypeId typeId) const;

  private:
    std::unordered_map<std::uint32_t, NodeVisual> m_visuals{};
    NodeVisual m_defaultVisual{};
};
} // namespace ScopeCanvas::Render::Theme

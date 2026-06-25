#pragma once

#include <glm/vec4.hpp>

namespace ScopeCanvas::Render {
struct NodeRenderStyle {
    glm::vec4 bodyColor{0.16F, 0.18F, 0.22F, 0.96F};
    glm::vec4 borderColor{0.35F, 0.39F, 0.48F, 1.0F};
    glm::vec4 selectionColor{0.95F, 0.67F, 0.25F, 1.0F};
    float borderThickness{1.5F};
    float cornerRadius{10.0F};
};
} // namespace ScopeCanvas::Render
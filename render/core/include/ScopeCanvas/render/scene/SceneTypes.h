#pragma once

#include <glm/glm.hpp>

namespace ScopeCanvas::Render::Scene {

struct NodeInstance {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec3 color;
};

} // namespace ScopeCanvas::Render::Scene
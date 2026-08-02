#pragma once

#include <glm/glm.hpp>

namespace ScopeCanvas::Engine::Render::Scene {

struct NodeInstance {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec3 color;
};

} // namespace ScopeCanvas::Engine::Render::Scene

#pragma once

#include <ScopeCanvas/core/Node.h>
#include <algorithm>
#include <glm/geometric.hpp>

namespace ScopeCanvas::Routing {

bool pointInNode(const glm::vec2& point, const Core::Node& node) {
    return point.x >= node.position.x && point.x <= node.position.x + node.size.x && point.y >= node.position.y &&
           point.y <= node.position.y + node.size.y;
}

float distanceToSegmentSquared(const glm::vec2& point, const glm::vec2& a, const glm::vec2& b) {
    const glm::vec2 ab = b - a;
    const float len2 = glm::dot(ab, ab);
    if (len2 <= 0.0001F) {
        const glm::vec2 d = point - a;
        return glm::dot(d, d);
    }
    const float t = std::clamp(glm::dot(point - a, ab) / len2, 0.0F, 1.0F);
    const glm::vec2 proj = a + ab * t;
    const glm::vec2 d = point - proj;
    return glm::dot(d, d);
}

} // namespace ScopeCanvas::Routing
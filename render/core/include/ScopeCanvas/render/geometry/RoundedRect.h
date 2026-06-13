#pragma once

#include <algorithm>
#include <cmath>
#include <glm/vec2.hpp>
#include <vector>

namespace ScopeCanvas::Render::Geometry {
inline constexpr float kPi = 3.14159265358979323846F;

inline std::vector<glm::vec2> roundedRectOutline(glm::vec2 position, glm::vec2 size, float radius, int segments = 10) {
    segments = std::max(1, segments);

    const float r = std::clamp(radius, 0.0F, std::min(size.x, size.y) * 0.5F);
    const glm::vec2 min = position;
    const glm::vec2 max = position + size;

    if (r <= 0.01F) {
        return {
            {min.x, min.y},
            {max.x, min.y},
            {max.x, max.y},
            {min.x, max.y},
        };
    }

    struct Corner {
        glm::vec2 center;
        float start;
        float end;
    };

    const Corner corners[] = {
        {{min.x + r, max.y - r}, kPi, kPi * 0.5F},
        {{max.x - r, max.y - r}, kPi * 0.5F, 0.0F},
        {{max.x - r, min.y + r}, 0.0F, -kPi * 0.5F},
        {{min.x + r, min.y + r}, -kPi * 0.5F, -kPi},
    };

    std::vector<glm::vec2> points;
    points.reserve(static_cast<std::size_t>(segments + 1) * 4U);

    for (const Corner& corner : corners) {
        for (int i = 0; i <= segments; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(segments);
            const float angle = corner.start + (corner.end - corner.start) * t;
            points.push_back(corner.center + glm::vec2{std::cos(angle), std::sin(angle)} * r);
        }
    }

    return points;
}

template <typename EmitTriangle>
void emitRoundedRectTriangles(glm::vec2 position, glm::vec2 size, float radius, EmitTriangle emitTriangle) {
    const std::vector<glm::vec2> points = roundedRectOutline(position, size, radius);
    if (points.size() < 3U)
        return;
    glm::vec2 center{0.0F, 0.0F};
    for (const glm::vec2& point : points)
        center += point;
    center /= static_cast<float>(points.size());
    for (std::size_t i = 0; i < points.size(); ++i)
        emitTriangle(center, points[i], points[(i + 1U) % points.size()]);
}
} // namespace ScopeCanvas::Render::Geometry

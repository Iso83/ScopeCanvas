#include <ScopeCanvas/render/renderers/EdgeRenderer.h>
#include <algorithm>
#include <cmath>

namespace ScopeCanvas::Render::Renderers {
std::vector<Core::Vec2> EdgeRenderer::buildEdgeGeometry(const Routing::EdgeRoute& route, int segmentsPerCurve) const {
    if (route.points.size() < 2) {
        return route.points;
    }

    const int segments = std::max(segmentsPerCurve, 1);
    std::vector<Core::Vec2> geometry;
    geometry.reserve(route.points.size() * static_cast<std::size_t>(segments));

    for (std::size_t i = 0; i + 1 < route.points.size(); ++i) {
        const Core::Vec2 p0 = route.points[i];
        const Core::Vec2 p3 = route.points[i + 1];

        Core::Vec2 p1{};
        Core::Vec2 p2{};
        computeBezierControls(p0, p3, p1, p2);

        appendBezierSamples(geometry, p0, p1, p2, p3, segments);
    }

    return geometry;
}

void EdgeRenderer::computeBezierControls(const Core::Vec2& p0, const Core::Vec2& p3, Core::Vec2& p1, Core::Vec2& p2) {
    const float dx = std::abs(p3.x - p0.x);
    const float controlDistance = std::max(80.0F, dx * 0.5F);
    p1 = {p0.x + controlDistance, p0.y};
    p2 = {p3.x - controlDistance, p3.y};
}

void EdgeRenderer::appendBezierSamples(std::vector<Core::Vec2>& points, const Core::Vec2& p0, const Core::Vec2& p1,
                                       const Core::Vec2& p2, const Core::Vec2& p3, int segments) {
    if (!points.empty()) {
        points.pop_back();
    }

    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const float omt = 1.0F - t;

        const float x =
            omt * omt * omt * p0.x + 3.0F * omt * omt * t * p1.x + 3.0F * omt * t * t * p2.x + t * t * t * p3.x;
        const float y =
            omt * omt * omt * p0.y + 3.0F * omt * omt * t * p1.y + 3.0F * omt * t * t * p2.y + t * t * t * p3.y;

        points.push_back({x, y});
    }
}
} // namespace ScopeCanvas::Render::Renderers

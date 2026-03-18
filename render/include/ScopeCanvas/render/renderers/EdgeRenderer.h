#pragma once

#include <ScopeCanvas/routing/EdgeRoute.h>
#include <vector>

namespace ScopeCanvas::Render::Renderers {
class EdgeRenderer {
  public:
    [[nodiscard]] std::vector<Core::Vec2> buildEdgeGeometry(const Routing::EdgeRoute& route,
                                                            int segmentsPerCurve = 20) const;

  private:
    static void computeBezierControls(const Core::Vec2& p0, const Core::Vec2& p3, Core::Vec2& p1, Core::Vec2& p2);

    static void appendBezierSamples(std::vector<Core::Vec2>& points, const Core::Vec2& p0, const Core::Vec2& p1,
                                    const Core::Vec2& p2, const Core::Vec2& p3, int segments);
};
} // namespace ScopeCanvas::Render::Renderers

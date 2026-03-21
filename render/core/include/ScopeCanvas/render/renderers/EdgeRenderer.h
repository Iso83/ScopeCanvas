#pragma once

#include <ScopeCanvas/routing/EdgeRoute.h>
#include <vector>

namespace ScopeCanvas::Render::Renderers {
class EdgeRenderer {
  public:
    [[nodiscard]] std::vector<glm::vec2> buildEdgeGeometry(const Routing::EdgeRoute& route,
                                                           int segmentsPerCurve = 20) const;

  private:
    static void computeBezierControls(const glm::vec2& p0, const glm::vec2& p3, glm::vec2& p1, glm::vec2& p2);

    static void appendBezierSamples(std::vector<glm::vec2>& points, const glm::vec2& p0, const glm::vec2& p1,
                                    const glm::vec2& p2, const glm::vec2& p3, int segments);
};
} // namespace ScopeCanvas::Render::Renderers

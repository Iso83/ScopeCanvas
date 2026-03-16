#pragma once

#include <vector>

#include "ScopeCanvasRouting/Routing/EdgeRoute.h"

namespace ScopeCanvas::Render::Renderers
{
class EdgeRenderer
{
public:
    [[nodiscard]] std::vector<Engine::Core::Vec2> buildEdgeGeometry(const Engine::Routing::EdgeRoute& route,
                                                                     int segmentsPerCurve = 20) const;

private:
    static void computeBezierControls(const Engine::Core::Vec2& p0,
                                      const Engine::Core::Vec2& p3,
                                      Engine::Core::Vec2& p1,
                                      Engine::Core::Vec2& p2);

    static void appendBezierSamples(std::vector<Engine::Core::Vec2>& points,
                                    const Engine::Core::Vec2& p0,
                                    const Engine::Core::Vec2& p1,
                                    const Engine::Core::Vec2& p2,
                                    const Engine::Core::Vec2& p3,
                                    int segments);
};
} // namespace ScopeCanvas::Render::Renderers

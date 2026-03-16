#include "ScopeCanvasEngineCore/Core/Vec2.h"
#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"
#include "ScopeCanvasRouting/Routing/EdgeRoute.h"

int main()
{
    using namespace ScopeCanvas::Engine;

    Routing::EdgeRoute route{};
    route.edgeId = Core::CanvasEdgeId{1};
    route.points.push_back(Core::Vec2{0.0F, 0.0F});
    route.points.push_back(Core::Vec2{10.0F, 10.0F});

    return route.points.size() == 2 ? 0 : 1;
}

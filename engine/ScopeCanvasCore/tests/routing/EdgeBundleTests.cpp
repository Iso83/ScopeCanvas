#include "ScopeCanvasEngineCore/Core/Vec2.h"
#include "ScopeCanvasEngineCore/Ids/CanvasIds.h"
#include "ScopeCanvasRouting/Routing/EdgeBundle.h"

int main()
{
    using namespace ScopeCanvas::Engine;

    Routing::EdgeBundle bundle{};
    bundle.id = Routing::EdgeBundleId{1};
    bundle.position = Core::Vec2{5.0F, 6.0F};
    bundle.edges.push_back(Core::CanvasEdgeId{1});
    bundle.edges.push_back(Core::CanvasEdgeId{2});

    if (!bundle.id.isValid())
    {
        return 1;
    }

    return bundle.edges.size() == 2 ? 0 : 1;
}

#include <vector>

#include "ScopeCanvasEngineCore/Core/DiagramModel.h"
#include "ScopeCanvasEngineCore/Layout/GridSettings.h"
#include "ScopeCanvasEngineCore/Layout/LayoutEngine.h"
#include "ScopeCanvasEngineCore/Layout/LayoutGroup.h"

namespace ScopeCanvas::Engine::Core
{
class TestLayoutEngine final : public LayoutEngine
{
public:
    void layout(DiagramModel& model) override
    {
        (void)model;
        didLayout = true;
    }

    void layoutNodes(DiagramModel& model, const std::vector<CanvasNodeId>& nodes) override
    {
        (void)model;
        didLayoutNodes = !nodes.empty();
    }

    bool didLayout{};
    bool didLayoutNodes{};
};
} // namespace ScopeCanvas::Engine::Core

int main()
{
    using namespace ScopeCanvas::Engine::Core;

    LayoutGroup group{};
    group.id = LayoutGroupId{1};
    group.nodes.push_back(CanvasNodeId{42});
    group.preserveInternalLayout = true;

    GridSettings grid{};
    grid.enabled = true;
    grid.cellSize = 10.0F;
    const Vec2 snapped = grid.snap(Vec2{12.0F, 26.0F});

    DiagramModel model{};
    TestLayoutEngine engine{};
    engine.layout(model);
    engine.layoutNodes(model, group.nodes);

    return (group.id.isValid() && !group.nodes.empty() && group.preserveInternalLayout &&
            snapped == Vec2{10.0F, 30.0F} && engine.didLayout && engine.didLayoutNodes)
               ? 0
               : 1;
}

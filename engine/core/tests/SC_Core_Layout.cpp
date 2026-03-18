#include "layout/GridSettings.h"

#include <ScopeCanvas/core/DiagramModel.h>
#include <ScopeCanvas/core/layout/LayoutEngine.h>
#include <ScopeCanvas/core/layout/LayoutGroup.h>
#include <TestAssert.h>
#include <vector>

namespace ScopeCanvas::Core {
class TestLayoutEngine final : public LayoutEngine {
  public:
    void layout(DiagramModel&) override {
        didLayout = true;
    }

    void layoutNodes(DiagramModel&, const std::vector<CanvasNodeId>& nodes) override {
        didLayoutNodes = !nodes.empty();
    }

    bool didLayout{};
    bool didLayoutNodes{};
};
} // namespace ScopeCanvas::Core

int main() {
    using namespace ScopeCanvas::Core;

    LayoutGroup group{};
    group.id = LayoutGroupId{1};
    group.nodes.push_back(CanvasNodeId{42});
    group.preserveInternalLayout = true;

    GridSettings grid{};
    grid.enabled = true;
    grid.cellSize = 10.0f;

    SC_TEST((grid.snap(Vec2{12.0f, 26.0f}) == Vec2{10.0f, 30.0f}));

    DiagramModel model{};
    TestLayoutEngine engine{};

    engine.layout(model);
    engine.layoutNodes(model, group.nodes);

    SC_TEST(group.id.isValid());
    SC_TEST(!group.nodes.empty());
    SC_TEST(group.preserveInternalLayout);
    SC_TEST(engine.didLayout);
    SC_TEST(engine.didLayoutNodes);

    return 0;
}
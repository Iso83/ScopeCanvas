#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/routing/EdgeBundle.h>
#include <TestAssert.h>
#include <glm/vec2.hpp>

int main() {
    using namespace ScopeCanvas;

    Routing::EdgeBundle bundle{};
    bundle.id = Routing::EdgeBundleId{1};
    bundle.position = glm::vec2{5.0f, 6.0f};

    bundle.edges.push_back(Core::CanvasEdgeId{1});
    bundle.edges.push_back(Core::CanvasEdgeId{2});

    SC_TEST(bundle.id.isValid());
    SC_TEST(bundle.edges.size() == 2);

    return 0;
}
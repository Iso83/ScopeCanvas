#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/TypeIds.h>
#include <TestAssert.h>

int main() {
    using namespace ScopeCanvas::Core;

    SC_TEST(CanvasNodeId{1}.isValid());
    SC_TEST(CanvasConnectorId{2}.isValid());
    SC_TEST(CanvasEdgeId{3}.isValid());
    SC_TEST(NodeTypeId{4}.isValid());
    SC_TEST(ConnectorTypeId{5}.isValid());
    SC_TEST(LayoutGroupId{6}.isValid());

    return 0;
}
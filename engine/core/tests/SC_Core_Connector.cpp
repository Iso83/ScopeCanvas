#include <ScopeCanvas/core/Connector.h>
#include <ScopeCanvas/core/Edge.h>
#include <TestAssert.h>

int main() {
    using namespace ScopeCanvas::Core;

    Connector connector{};
    connector.id = CanvasConnectorId{2};
    connector.nodeId = CanvasNodeId{1};
    connector.typeId = ConnectorTypeId{11};

    const Edge edge{CanvasEdgeId{3}, connector.id, CanvasConnectorId{4}};
    connector.edges.push_back(edge.id);

    SC_TEST(connector.id.isValid());
    SC_TEST(connector.nodeId.isValid());
    SC_TEST(connector.typeId.isValid());
    SC_TEST(!connector.edges.empty());
    SC_TEST(connector.edges.front() == edge.id);

    return 0;
}
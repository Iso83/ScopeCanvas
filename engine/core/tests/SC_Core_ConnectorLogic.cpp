#include "logic/ConnectorLogic.h"

#include <ScopeCanvas/core/GraphDocument.h>
#include <TestAssert.h>

int main() {
    using namespace ScopeCanvas::Core;

    GraphDocument model{};

    const auto n1 = model.createNode(NodeTypeId{1});
    const auto n2 = model.createNode(NodeTypeId{2});

    const Node* node1 = model.getNode(n1);
    const Node* node2 = model.getNode(n2);

    SC_TEST(node1 && node2);
    SC_TEST(node1->connectors.size() >= 2);
    SC_TEST(!node2->connectors.empty());

    const auto a = node1->connectors[0];
    const auto b = node1->connectors[1];
    const auto c = node2->connectors[0];

    SC_TEST(ConnectorLogic::sameNode(model, a, b));

    const auto edge = model.connect(a, c);

    SC_TEST(edge.isValid());
    SC_TEST(ConnectorLogic::edgeExists(model, a, c));

    SC_TEST(!ConnectorLogic::canConnect(model, a, c));
    SC_TEST(!ConnectorLogic::canConnect(model, a, a));

    return 0;
}
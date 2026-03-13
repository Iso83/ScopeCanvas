#include "demo/Sha256Demo.h"

#include "Engine/NodeTypes.h"

#include <array>
#include <string>
#include <vector>

namespace {
Node *createCustomNode(DiagramModel &graph,
    const std::string &title,
    const glm::vec2 &position,
    const glm::vec2 &size,
    int inputCount,
    int outputCount) {
    const std::vector<ConnectorTemplate> templates =
        createConnectorTemplatesForType(static_cast<size_t>(inputCount), static_cast<size_t>(outputCount));
    return graph.createNodeWithConnectors(position, size, templates, "Custom", title);
}

void connectIfPossible(DiagramModel &graph, const Node *from, int fromIx, const Node *to, int toIx) {
    if (from == nullptr || to == nullptr) {
        return;
    }

    if (fromIx < 0 || toIx < 0) {
        return;
    }

    const int outStart = 3;
    if (static_cast<size_t>(outStart + fromIx) >= from->connectors.size()) {
        return;
    }
    if (static_cast<size_t>(toIx) >= to->connectors.size()) {
        return;
    }

    graph.createEdge(from->id,
        from->connectors[static_cast<size_t>(outStart + fromIx)].id,
        to->id,
        to->connectors[static_cast<size_t>(toIx)].id);
}
}

void Sha256Demo::Build(DiagramModel &graph) {
    graph.clear();

    Node *inputData = createCustomNode(graph, "Data Input", { -780.0f, -90.0f }, { 220.0f, 120.0f }, 1, 1);
    Node *prime = createCustomNode(graph, "Prime Constant", { -780.0f, 90.0f }, { 220.0f, 100.0f }, 0, 1);

    Node *loopA = createCustomNode(graph, "Loop 1: Sigma/Choice", { -440.0f, -150.0f }, { 280.0f, 180.0f }, 3, 2);
    Node *loopB = createCustomNode(graph, "Loop 2: Mix/Add", { -100.0f, -150.0f }, { 280.0f, 180.0f }, 3, 2);
    Node *loopC = createCustomNode(graph, "Loop 3: Rotate/Finalize", { 240.0f, -150.0f }, { 300.0f, 180.0f }, 3, 2);

    Node *trueBit = createCustomNode(graph, "Const True Bit (OR => true)", { -440.0f, 130.0f }, { 280.0f, 90.0f }, 0, 1);
    Node *falseBit = createCustomNode(graph, "Const False Bit (AND => false)", { -100.0f, 130.0f }, { 300.0f, 90.0f }, 0, 1);
    Node *bitShift = createCustomNode(graph, "BitShift (>>) + Rotate", { 240.0f, 120.0f }, { 300.0f, 100.0f }, 2, 1);

    Node *outputHash = createCustomNode(graph, "SHA256 Output", { 620.0f, -40.0f }, { 240.0f, 120.0f }, 2, 1);

    connectIfPossible(graph, inputData, 0, loopA, 0);
    connectIfPossible(graph, prime, 0, loopA, 1);

    connectIfPossible(graph, loopA, 0, loopB, 0);
    connectIfPossible(graph, loopA, 1, loopB, 1);
    connectIfPossible(graph, trueBit, 0, loopB, 2);

    connectIfPossible(graph, loopB, 0, loopC, 0);
    connectIfPossible(graph, loopB, 1, loopC, 1);
    connectIfPossible(graph, falseBit, 0, loopC, 2);

    connectIfPossible(graph, loopC, 0, outputHash, 0);
    connectIfPossible(graph, bitShift, 0, outputHash, 1);

    connectIfPossible(graph, loopA, 0, bitShift, 0);
    connectIfPossible(graph, loopC, 1, bitShift, 1);

    Group *group = graph.createGroup();
    if (group != nullptr && loopA != nullptr && loopB != nullptr && loopC != nullptr) {
        graph.addNodeToGroup(loopA->id, group->id);
        graph.addNodeToGroup(loopB->id, group->id);
        graph.addNodeToGroup(loopC->id, group->id);
    }

    graph.syncIdCounters();
}

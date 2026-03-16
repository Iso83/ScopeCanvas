#include "demo/Sha256Demo.h"

#include "Engine/NodeTypes.h"

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

    const int outStart = 3;
    if (fromIx < 0 || toIx < 0) {
        return;
    }
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

void setAllMaxConnections(Node *node, int maxConnections) {
    if (node == nullptr) {
        return;
    }

    for (Connector &connector : node->connectors) {
        connector.maxConnections = maxConnections;
    }
}
}

void Sha256Demo::Build(DiagramModel &graph) {
    graph.clear();

    Node *dataBlock = createCustomNode(graph, "Bits Container W[t]", { -920.0f, -240.0f }, { 320.0f, 520.0f }, 8, 8);
    if (dataBlock != nullptr) {
        dataBlock->allowChildren = true;
    }

    std::vector<Node *> bits;
    bits.reserve(8);
    for (int i = 0; i < 8; ++i) {
        const float y = -200.0f + static_cast<float>(i) * 55.0f;
        bits.push_back(createCustomNode(graph,
            "Bit[" + std::to_string(i) + "] step " + std::to_string(i),
            { -860.0f, y },
            { 220.0f, 42.0f },
            0,
            1));
    }

    Node *prime = createCustomNode(graph, "Prime K[t]", { -560.0f, 140.0f }, { 190.0f, 80.0f }, 0, 1);
    Node *trueBit = createCustomNode(graph, "Const True Bit (OR=1)", { -560.0f, 240.0f }, { 190.0f, 80.0f }, 0, 1);
    Node *falseBit = createCustomNode(graph, "Const False Bit (AND=0)", { -330.0f, 240.0f }, { 210.0f, 80.0f }, 0, 1);

    Node *loopA = createCustomNode(graph, "Loop#1 Σ/Ch", { -560.0f, -120.0f }, { 260.0f, 180.0f }, 4, 2);
    Node *loopB = createCustomNode(graph, "Loop#2 Mix/Add", { -220.0f, -120.0f }, { 260.0f, 180.0f }, 4, 2);
    Node *loopC = createCustomNode(graph, "Loop#3 Rotate/Finalize", { 120.0f, -120.0f }, { 300.0f, 180.0f }, 4, 2);
    if (loopA != nullptr) {
        loopA->allowChildren = true;
    }

    Node *bitShift = createCustomNode(graph, "BitShift >>> / ROTR", { 120.0f, 120.0f }, { 300.0f, 100.0f }, 2, 1);
    Node *outputHash = createCustomNode(graph, "Hash Out H[t]", { 500.0f, -20.0f }, { 220.0f, 120.0f }, 2, 1);

    setAllMaxConnections(dataBlock, 4);
    setAllMaxConnections(loopA, 4);
    setAllMaxConnections(loopB, 4);
    setAllMaxConnections(loopC, 4);

    for (int i = 0; i < 8; ++i) {
        connectIfPossible(graph, bits[static_cast<size_t>(i)], 0, dataBlock, i);
    }

    for (int i = 0; i < 4; ++i) {
        connectIfPossible(graph, dataBlock, i, loopA, i);
    }

    connectIfPossible(graph, prime, 0, loopB, 3);
    connectIfPossible(graph, trueBit, 0, loopB, 2);
    connectIfPossible(graph, falseBit, 0, loopC, 3);

    connectIfPossible(graph, loopA, 0, loopB, 0);
    connectIfPossible(graph, loopA, 1, loopB, 1);
    connectIfPossible(graph, loopB, 0, loopC, 0);
    connectIfPossible(graph, loopB, 1, loopC, 1);

    connectIfPossible(graph, loopA, 1, bitShift, 0);
    connectIfPossible(graph, loopC, 1, bitShift, 1);

    connectIfPossible(graph, loopC, 0, outputHash, 0);
    connectIfPossible(graph, bitShift, 0, outputHash, 1);

    if (dataBlock != nullptr) {
        for (Node *bitNode : bits) {
            if (bitNode != nullptr) {
                graph.addChildNode(dataBlock->id, bitNode->id);
            }
        }
        graph.setNodeCollapsed(dataBlock->id, true);
    }

    if (loopA != nullptr && loopB != nullptr && loopC != nullptr) {
        graph.addChildNode(loopA->id, loopB->id);
        graph.addChildNode(loopA->id, loopC->id);
    }

    graph.syncIdCounters();
}

#include "Engine/LayoutEngine.h"

#include <algorithm>
#include <queue>
#include <unordered_map>

void LayoutEngine::applyLayout(DiagramModel& graph) {
    std::unordered_map<uint32_t, int> nodeLevels;
    nodeLevels.reserve(graph.nodes().size());

    for (const Node& node : graph.nodes()) {
        nodeLevels[node.id] = 0;
    }

    std::unordered_map<uint32_t, int> incomingCounts;
    incomingCounts.reserve(graph.nodes().size());
    for (const Node& node : graph.nodes()) {
        incomingCounts[node.id] = 0;
    }

    for (const Edge& edge : graph.edges()) {
        auto incomingIt = incomingCounts.find(edge.toNode);
        if (incomingIt != incomingCounts.end()) {
            ++incomingIt->second;
        }
    }

    std::queue<uint32_t> traversalQueue;
    for (const Node& node : graph.nodes()) {
        auto incomingIt = incomingCounts.find(node.id);
        if (incomingIt != incomingCounts.end() && incomingIt->second == 0) {
            traversalQueue.push(node.id);
            nodeLevels[node.id] = 0;
        }
    }

    while (!traversalQueue.empty()) {
        const uint32_t nodeId = traversalQueue.front();
        traversalQueue.pop();

        const int currentLevel = nodeLevels[nodeId];
        for (const Edge& edge : graph.edges()) {
            if (edge.fromNode != nodeId) {
                continue;
            }

            auto levelIt = nodeLevels.find(edge.toNode);
            if (levelIt == nodeLevels.end()) {
                continue;
            }

            levelIt->second = std::max(levelIt->second, currentLevel + 1);

            auto incomingIt = incomingCounts.find(edge.toNode);
            if (incomingIt == incomingCounts.end()) {
                continue;
            }

            --incomingIt->second;
            if (incomingIt->second == 0) {
                traversalQueue.push(edge.toNode);
            }
        }
    }

    constexpr float verticalSpacing = 200.0f;
    constexpr float horizontalSpacing = 240.0f;
    std::unordered_map<int, size_t> levelNodeIndices;
    levelNodeIndices.reserve(graph.nodes().size());

    for (Node& node : graph.nodes()) {
        auto levelIt = nodeLevels.find(node.id);
        if (levelIt == nodeLevels.end()) {
            continue;
        }

        const int level = levelIt->second;
        const size_t nodeIndexInLevel = levelNodeIndices[level]++;
        node.position.x = static_cast<float>(nodeIndexInLevel) * horizontalSpacing;
        node.position.y = static_cast<float>(level) * verticalSpacing;
    }
}

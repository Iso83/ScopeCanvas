#include <ScopeCanvas/render/scene/SceneBuilder.h>
#include <unordered_set>

namespace ScopeCanvas::Render::Scene {
namespace {
constexpr std::uint32_t kMaxConsecutiveMissingNodeIds = 128;

std::vector<Core::CanvasNodeId> collectNodeIds(const Core::DiagramModel& model,
                                               const std::vector<Routing::EdgeRoute>& edgeRoutes) {
    std::unordered_set<std::uint32_t> uniqueIds;

    for (const Routing::EdgeRoute& route : edgeRoutes) {
        const Core::Edge* edge = model.getEdge(route.edgeId);
        if (edge == nullptr) {
            continue;
        }

        const Core::Connector* from = model.getConnector(edge->fromConnector);
        const Core::Connector* to = model.getConnector(edge->toConnector);
        if (from != nullptr && from->nodeId.isValid())
            uniqueIds.insert(from->nodeId.value());

        if (to != nullptr && to->nodeId.isValid())
            uniqueIds.insert(to->nodeId.value());
    }

    std::uint32_t missStreak = 0;
    for (std::uint32_t probe = 1; missStreak < kMaxConsecutiveMissingNodeIds; ++probe) {
        if (model.getNode(Core::CanvasNodeId{probe}) != nullptr) {
            uniqueIds.insert(probe);
            missStreak = 0;
        } else {
            ++missStreak;
        }
    }

    std::vector<Core::CanvasNodeId> result;
    result.reserve(uniqueIds.size());
    for (const std::uint32_t id : uniqueIds) {
        result.emplace_back(id);
    }

    return result;
}

glm::vec2 computeConnectorAnchor(const Core::Node& node, std::size_t connectorIndex) {
    const float count = static_cast<float>(node.connectors.size() + 1U);
    const float step = node.size.y / count;
    const float y = node.position.y + step * static_cast<float>(connectorIndex + 1U);
    return {node.position.x + node.size.x, y};
}
} // namespace

RenderScene SceneBuilder::build(const Core::DiagramModel& model,
                                const std::vector<Routing::EdgeRoute>& edgeRoutes) const {
    RenderScene scene{};

    const std::vector<Core::CanvasNodeId> nodeIds = collectNodeIds(model, edgeRoutes);
    scene.nodes.reserve(nodeIds.size());

    for (Core::CanvasNodeId nodeId : nodeIds) {
        const Core::Node* node = model.getNode(nodeId);
        if (node == nullptr) {
            continue;
        }

        scene.nodes.push_back({node->id, node->position, node->size});

        for (std::size_t i = 0; i < node->connectors.size(); ++i) {
            const Core::CanvasConnectorId connectorId = node->connectors[i];
            scene.connectorAnchors.push_back({connectorId, node->id, computeConnectorAnchor(*node, i)});
        }
    }

    scene.edges.reserve(edgeRoutes.size());
    for (const Routing::EdgeRoute& route : edgeRoutes) {
        scene.edges.push_back({route.edgeId, route.points});
    }

    return scene;
}
} // namespace ScopeCanvas::Render::Scene

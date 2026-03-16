#include "Scene/SceneBuilder.h"

#include <unordered_set>

namespace ScopeCanvas::RenderGL::Scene
{
namespace
{
constexpr std::uint32_t kMaxConsecutiveMissingNodeIds = 128;

std::vector<Engine::Core::CanvasNodeId> collectNodeIds(const Engine::Core::DiagramModel& model,
                                                       const std::vector<Engine::Routing::EdgeRoute>& edgeRoutes)
{
    std::unordered_set<std::uint32_t> uniqueIds;

    for (const Engine::Routing::EdgeRoute& route : edgeRoutes)
    {
        const Engine::Core::Edge* edge = model.getEdge(route.edgeId);
        if (edge == nullptr)
        {
            continue;
        }

        const Engine::Core::Connector* from = model.getConnector(edge->fromConnector);
        const Engine::Core::Connector* to = model.getConnector(edge->toConnector);
        if (from != nullptr && from->nodeId.isValid())
        {
            uniqueIds.insert(from->nodeId.value());
        }
        if (to != nullptr && to->nodeId.isValid())
        {
            uniqueIds.insert(to->nodeId.value());
        }
    }

    std::uint32_t missStreak = 0;
    for (std::uint32_t probe = 1; missStreak < kMaxConsecutiveMissingNodeIds; ++probe)
    {
        if (model.getNode(Engine::Core::CanvasNodeId{probe}) != nullptr)
        {
            uniqueIds.insert(probe);
            missStreak = 0;
        }
        else
        {
            ++missStreak;
        }
    }

    std::vector<Engine::Core::CanvasNodeId> result;
    result.reserve(uniqueIds.size());
    for (const std::uint32_t id : uniqueIds)
    {
        result.emplace_back(id);
    }

    return result;
}

Engine::Core::Vec2 computeConnectorAnchor(const Engine::Core::Node& node, std::size_t connectorIndex)
{
    const float count = static_cast<float>(node.connectors.size() + 1U);
    const float step = node.size.y / count;
    const float y = node.position.y + step * static_cast<float>(connectorIndex + 1U);
    return {node.position.x + node.size.x, y};
}
} // namespace

RenderScene SceneBuilder::build(const Engine::Core::DiagramModel& model,
                                const std::vector<Engine::Routing::EdgeRoute>& edgeRoutes) const
{
    RenderScene scene{};

    const std::vector<Engine::Core::CanvasNodeId> nodeIds = collectNodeIds(model, edgeRoutes);
    scene.nodes.reserve(nodeIds.size());

    for (Engine::Core::CanvasNodeId nodeId : nodeIds)
    {
        const Engine::Core::Node* node = model.getNode(nodeId);
        if (node == nullptr)
        {
            continue;
        }

        scene.nodes.push_back({node->id, node->position, node->size});

        for (std::size_t i = 0; i < node->connectors.size(); ++i)
        {
            const Engine::Core::CanvasConnectorId connectorId = node->connectors[i];
            scene.connectorAnchors.push_back({connectorId, node->id, computeConnectorAnchor(*node, i)});
        }
    }

    scene.edges.reserve(edgeRoutes.size());
    for (const Engine::Routing::EdgeRoute& route : edgeRoutes)
    {
        scene.edges.push_back({route.edgeId, route.points});
    }

    return scene;
}
} // namespace ScopeCanvas::RenderGL::Scene

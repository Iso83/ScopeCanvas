#include <ScopeCanvas/render/scene/SceneBuilder.h>
#include <algorithm>
#include <glm/vec4.hpp>
#include <limits>
#include <unordered_set>

namespace ScopeCanvas::Render::Scene {
namespace {
constexpr std::uint32_t kMaxConsecutiveMissingNodeIds = 128;
constexpr float kCullPadding = 64.0F;

struct WorldBounds {
    glm::vec2 min{};
    glm::vec2 max{};
};

WorldBounds computeWorldBounds(const Camera::Camera2D& camera) {
    const glm::mat4 invViewProjection = camera.invViewProjection();
    const glm::vec4 corners[] = {
        {-1.0F, -1.0F, 0.0F, 1.0F},
        {1.0F, -1.0F, 0.0F, 1.0F},
        {-1.0F, 1.0F, 0.0F, 1.0F},
        {1.0F, 1.0F, 0.0F, 1.0F},
    };

    WorldBounds bounds{
        .min{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()},
        .max{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()},
    };

    for (const glm::vec4& corner : corners) {
        const glm::vec4 world = invViewProjection * corner;
        const glm::vec2 p = glm::vec2(world) / world.w;
        bounds.min.x = std::min(bounds.min.x, p.x);
        bounds.min.y = std::min(bounds.min.y, p.y);
        bounds.max.x = std::max(bounds.max.x, p.x);
        bounds.max.y = std::max(bounds.max.y, p.y);
    }

    bounds.min -= glm::vec2(kCullPadding, kCullPadding);
    bounds.max += glm::vec2(kCullPadding, kCullPadding);
    return bounds;
}

bool intersectsBounds(const glm::vec2& position, const glm::vec2& size, const WorldBounds& bounds) {
    return !(position.x + size.x < bounds.min.x || position.y + size.y < bounds.min.y || position.x > bounds.max.x ||
             position.y > bounds.max.y);
}

std::vector<Core::CanvasNodeId> collectNodeIds(const Core::GraphDocument& model,
                                               const std::vector<Routing::EdgeRoute>& edgeRoutes) {
    std::unordered_set<std::uint32_t> uniqueIds;

    for (const Routing::EdgeRoute& route : edgeRoutes) {
        const Core::Edge* edge = model.getEdge(route.edgeId);
        if (edge == nullptr) {
            continue;
        }

        if (const Core::Connector* from = model.getConnector(edge->fromConnector);
            from != nullptr && from->nodeId.isValid()) {
            uniqueIds.insert(from->nodeId.value());
        }
        if (const Core::Connector* to = model.getConnector(edge->toConnector); to != nullptr && to->nodeId.isValid()) {
            uniqueIds.insert(to->nodeId.value());
        }
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
    std::sort(result.begin(), result.end(), [](Core::CanvasNodeId a, Core::CanvasNodeId b) { return a.value() < b.value(); });
    return result;
}

glm::vec2 computeConnectorAnchor(const Core::Node& node, std::size_t connectorIndex) {
    const bool output = (connectorIndex % 2U) == 1U;
    const std::size_t sideIndex = connectorIndex / 2U;
    const std::size_t sideCount = output ? node.connectors.size() / 2U : (node.connectors.size() + 1U) / 2U;
    constexpr float kHeaderHeight = 24.0F;
    constexpr float kVerticalInset = 12.0F;
    const float bodyMinY = node.position.y + kVerticalInset;
    const float bodyMaxY = std::max(bodyMinY + 1.0F, node.position.y + node.size.y - kHeaderHeight - kVerticalInset);
    const float bodyHeight = std::max(bodyMaxY - bodyMinY, 1.0F);
    const float step = bodyHeight / static_cast<float>(sideCount + 1U);
    const float y = bodyMinY + step * static_cast<float>(sideIndex + 1U);
    return {output ? node.position.x + node.size.x : node.position.x, y};
}
} // namespace

RenderScene SceneBuilder::build(const Core::GraphDocument& model, const std::vector<Routing::EdgeRoute>& edgeRoutes,
                                const Camera::Camera2D& camera) const {
    RenderScene scene{};
    const WorldBounds bounds = computeWorldBounds(camera);

    const std::vector<Core::CanvasNodeId> nodeIds = collectNodeIds(model, edgeRoutes);
    scene.nodes.reserve(nodeIds.size());

    std::unordered_set<std::uint32_t> visibleNodeValues;

    for (Core::CanvasNodeId nodeId : nodeIds) {
        const Core::Node* node = model.getNode(nodeId);
        if (node == nullptr || !intersectsBounds(node->position, node->size, bounds)) {
            continue;
        }

        visibleNodeValues.insert(node->id.value());
        scene.nodes.push_back(
            {node->id, node->typeId, node->position, node->size, static_cast<std::uint32_t>(node->connectors.size())});

        for (std::size_t i = 0; i < node->connectors.size(); ++i) {
            const Core::CanvasConnectorId connectorId = node->connectors[i];
            scene.connectorAnchors.push_back({connectorId, node->id, computeConnectorAnchor(*node, i), (i % 2U) == 1U});
        }
    }

    scene.edges.reserve(edgeRoutes.size());
    for (const Routing::EdgeRoute& route : edgeRoutes) {
        const Core::Edge* edge = model.getEdge(route.edgeId);
        if (edge == nullptr) {
            continue;
        }

        bool endpointVisible = false;
        if (const Core::Connector* from = model.getConnector(edge->fromConnector);
            from != nullptr && from->nodeId.isValid()) {
            endpointVisible = visibleNodeValues.contains(from->nodeId.value());
        }
        if (!endpointVisible) {
            if (const Core::Connector* to = model.getConnector(edge->toConnector);
                to != nullptr && to->nodeId.isValid()) {
                endpointVisible = visibleNodeValues.contains(to->nodeId.value());
            }
        }

        if (!endpointVisible) {
            continue;
        }
        scene.edges.push_back({route.edgeId, route.points});
    }

    return scene;
}
} // namespace ScopeCanvas::Render::Scene

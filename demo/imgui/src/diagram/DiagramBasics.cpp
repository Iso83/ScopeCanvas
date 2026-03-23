#include "diagram/DiagramBasics.h"

#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>

namespace ScopeCanvas::Studio {
namespace {
glm::vec2 connectorAnchor(const Core::Node& node, std::size_t index) {
    const bool output = (index % 2U) == 1U;
    const std::size_t sideIndex = index / 2U;
    const std::size_t sideCount = output ? node.connectors.size() / 2U : (node.connectors.size() + 1U) / 2U;
    constexpr float headerHeight = 24.0F;
    constexpr float verticalInset = 12.0F;
    const float bodyMinY = node.position.y + verticalInset;
    const float bodyMaxY = std::max(bodyMinY + 1.0F, node.position.y + node.size.y - headerHeight - verticalInset);
    const float step = std::max(bodyMaxY - bodyMinY, 1.0F) / static_cast<float>(sideCount + 1U);
    const float y = bodyMinY + step * static_cast<float>(sideIndex + 1U);
    return {output ? node.position.x + node.size.x : node.position.x, y};
}

std::vector<glm::vec2> buildFallbackRoutePoints(const Core::Node& fromNode, std::size_t fromIndex, const Core::Node& toNode,
                                                std::size_t toIndex) {
    const glm::vec2 start = connectorAnchor(fromNode, fromIndex);
    const glm::vec2 end = connectorAnchor(toNode, toIndex);
    const glm::vec2 startNormal = (fromIndex % 2U) == 1U ? glm::vec2(1.0F, 0.0F) : glm::vec2(-1.0F, 0.0F);
    const glm::vec2 endNormal = (toIndex % 2U) == 1U ? glm::vec2(1.0F, 0.0F) : glm::vec2(-1.0F, 0.0F);
    constexpr float kBreakout = 48.0F;
    constexpr float kDetourMargin = 36.0F;

    const glm::vec2 startStub = start + startNormal * kBreakout;
    const glm::vec2 endStub = end + endNormal * kBreakout;
    const bool needsDetour =
        startNormal.x > 0.0F && endNormal.x < 0.0F && end.x <= start.x + kBreakout * 1.5F;
    if (needsDetour) {
        const float fromTop = fromNode.position.y;
        const float fromBottom = fromNode.position.y + fromNode.size.y;
        const float toTop = toNode.position.y;
        const float toBottom = toNode.position.y + toNode.size.y;
        const float detourY = end.y >= start.y ? std::max(fromBottom, toBottom) + kDetourMargin
                                               : std::min(fromTop, toTop) - kDetourMargin;
        return {start, startStub, {startStub.x, detourY}, {endStub.x, detourY}, endStub, end};
    }

    if (glm::dot(end - start, startNormal) < kBreakout) {
        return {start, startStub, {startStub.x, end.y}, endStub, end};
    }

    return {start, end};
}
} // namespace
DiagramBasics::DiagramBasics() {
    (void)createNode(Core::NodeTypeId{10}, {-260.0F, -96.0F});
    (void)createNode(Core::NodeTypeId{11}, {32.0F, -160.0F});
    (void)createNode(Core::NodeTypeId{15}, {48.0F, 80.0F});
    (void)createNode(Core::NodeTypeId{4}, {336.0F, -24.0F});

    if (m_nodeIds.size() >= 4U) {
        const Core::Node* n0 = m_model.getNode(m_nodeIds[0]);
        const Core::Node* n1 = m_model.getNode(m_nodeIds[1]);
        const Core::Node* n2 = m_model.getNode(m_nodeIds[2]);
        const Core::Node* n3 = m_model.getNode(m_nodeIds[3]);
        if (n0 != nullptr && n1 != nullptr && n2 != nullptr && n3 != nullptr) {
            connect(n0->connectors[1], n1->connectors[0]);
            connect(n0->connectors[1], n2->connectors[0]);
            connect(n2->connectors[1], n3->connectors[0]);
        }
    }
}

Core::GraphDocument& DiagramBasics::model() {
    return m_model;
}
const Core::GraphDocument& DiagramBasics::model() const {
    return m_model;
}
GridSettings& DiagramBasics::gridSettings() {
    return m_grid;
}
const GridSettings& DiagramBasics::gridSettings() const {
    return m_grid;
}
std::vector<Core::CanvasNodeId>& DiagramBasics::nodeIds() {
    return m_nodeIds;
}
const std::vector<Core::CanvasNodeId>& DiagramBasics::nodeIds() const {
    return m_nodeIds;
}
std::vector<Core::CanvasEdgeId>& DiagramBasics::edgeIds() {
    return m_edgeIds;
}
const std::vector<Core::CanvasEdgeId>& DiagramBasics::edgeIds() const {
    return m_edgeIds;
}
std::vector<Core::CanvasNodeId>& DiagramBasics::selectedNodeIds() {
    return m_selectedNodeIds;
}
const std::vector<Core::CanvasNodeId>& DiagramBasics::selectedNodeIds() const {
    return m_selectedNodeIds;
}

Core::CanvasNodeId DiagramBasics::createNode(Core::NodeTypeId typeId, glm::vec2 position) {
    const Core::CanvasNodeId id = m_model.createNode(typeId);
    if (Core::Node* node = m_model.getNode(id); node != nullptr) {
        if (m_grid.snapEnabled) {
            const float s = m_grid.cellSize;
            position.x = std::round(position.x / s) * s;
            position.y = std::round(position.y / s) * s;
        }
        node->setPosition(position);
        node->setSize({180.0F, 110.0F});
    }
    m_nodeIds.push_back(id);
    return id;
}

Core::CanvasEdgeId DiagramBasics::connect(Core::CanvasConnectorId a, Core::CanvasConnectorId b) {
    const Core::CanvasEdgeId edgeId = m_model.connect(a, b);
    if (edgeId.isValid()) {
        m_edgeIds.push_back(edgeId);
    }
    return edgeId;
}

void DiagramBasics::deleteEdge(Core::CanvasEdgeId edgeId) {
    if (!edgeId.isValid()) {
        return;
    }
    m_model.disconnect(edgeId);
    m_edgeIds.erase(std::remove(m_edgeIds.begin(), m_edgeIds.end(), edgeId), m_edgeIds.end());
}

void DiagramBasics::deleteNode(Core::CanvasNodeId nodeId) {
    m_model.removeNode(nodeId);
    m_nodeIds.erase(std::remove(m_nodeIds.begin(), m_nodeIds.end(), nodeId), m_nodeIds.end());
    m_selectedNodeIds.erase(std::remove(m_selectedNodeIds.begin(), m_selectedNodeIds.end(), nodeId), m_selectedNodeIds.end());
    m_edgeIds.erase(std::remove_if(m_edgeIds.begin(), m_edgeIds.end(),
                                   [this](Core::CanvasEdgeId id) { return m_model.getEdge(id) == nullptr; }),
                    m_edgeIds.end());
}

void DiagramBasics::clearSelection() {
    m_selectedNodeIds.clear();
}

void DiagramBasics::setSelection(const std::vector<Core::CanvasNodeId>& nodeIds) {
    m_selectedNodeIds = nodeIds;
}

void DiagramBasics::setNodeSelected(Core::CanvasNodeId nodeId, bool selected) {
    const auto it = std::find(m_selectedNodeIds.begin(), m_selectedNodeIds.end(), nodeId);
    if (selected) {
        if (it == m_selectedNodeIds.end()) {
            m_selectedNodeIds.push_back(nodeId);
        }
    } else if (it != m_selectedNodeIds.end()) {
        m_selectedNodeIds.erase(it);
    }
}

bool DiagramBasics::isNodeSelected(Core::CanvasNodeId nodeId) const {
    return std::find(m_selectedNodeIds.begin(), m_selectedNodeIds.end(), nodeId) != m_selectedNodeIds.end();
}

std::vector<Routing::EdgeRoute> DiagramBasics::routeAllEdges() const {
    Routing::EdgeRouter router;
    std::vector<Routing::EdgeRoute> routes = router.routeAll(m_model);
    if (routes.empty()) {
        routes.reserve(m_edgeIds.size());
        for (Core::CanvasEdgeId id : m_edgeIds) {
            const Core::Edge* edge = m_model.getEdge(id);
            if (edge == nullptr) {
                continue;
            }
            const Core::Connector* c0 = m_model.getConnector(edge->fromConnector);
            const Core::Connector* c1 = m_model.getConnector(edge->toConnector);
            if (c0 == nullptr || c1 == nullptr) {
                continue;
            }
            const Core::Node* n0 = m_model.getNode(c0->nodeId);
            const Core::Node* n1 = m_model.getNode(c1->nodeId);
            if (n0 == nullptr || n1 == nullptr) {
                continue;
            }
            Routing::EdgeRoute route{};
            route.edgeId = id;
            std::size_t fromIndex = 0;
            while (fromIndex < n0->connectors.size() && n0->connectors[fromIndex] != edge->fromConnector) {
                ++fromIndex;
            }
            std::size_t toIndex = 0;
            while (toIndex < n1->connectors.size() && n1->connectors[toIndex] != edge->toConnector) {
                ++toIndex;
            }
            route.points = buildFallbackRoutePoints(*n0, std::min(fromIndex, n0->connectors.size() - 1U), *n1,
                                                    std::min(toIndex, n1->connectors.size() - 1U));
            route.preferStraightSegments = route.points.size() > 2U;
            routes.push_back(route);
        }
    }
    return routes;
}
} // namespace ScopeCanvas::Studio

#include "diagram/DiagramBasics.h"

#include <algorithm>
#include <cmath>

namespace ScopeCanvas::Studio {
DiagramBasics::DiagramBasics() {
    (void)createNode(Core::NodeTypeId{1}, {-220.0F, -40.0F});
    (void)createNode(Core::NodeTypeId{2}, {20.0F, 60.0F});
    (void)createNode(Core::NodeTypeId{4}, {280.0F, -80.0F});
}

Core::DiagramModel& DiagramBasics::model() {
    return m_model;
}
const Core::DiagramModel& DiagramBasics::model() const {
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

Core::CanvasNodeId DiagramBasics::createNode(Core::NodeTypeId typeId, Core::Vec2 position) {
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

void DiagramBasics::deleteNode(Core::CanvasNodeId nodeId) {
    m_model.removeNode(nodeId);
    m_nodeIds.erase(std::remove(m_nodeIds.begin(), m_nodeIds.end(), nodeId), m_nodeIds.end());
    m_edgeIds.erase(std::remove_if(m_edgeIds.begin(), m_edgeIds.end(),
                                   [this](Core::CanvasEdgeId id) { return m_model.getEdge(id) == nullptr; }),
                    m_edgeIds.end());
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
            route.points.push_back({n0->position.x + n0->size.x, n0->position.y + n0->size.y * 0.5F});
            route.points.push_back({n1->position.x, n1->position.y + n1->size.y * 0.5F});
            routes.push_back(route);
        }
    }
    return routes;
}
} // namespace ScopeCanvas::Studio

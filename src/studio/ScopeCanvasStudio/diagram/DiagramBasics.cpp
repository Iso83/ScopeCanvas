#include "diagram/DiagramBasics.h"

#include <algorithm>
#include <cmath>

namespace ScopeCanvas::Studio
{
DiagramBasics::DiagramBasics()
{
    (void)createNode(Engine::Core::NodeTypeId{1}, {-220.0F, -40.0F});
    (void)createNode(Engine::Core::NodeTypeId{2}, {20.0F, 60.0F});
    (void)createNode(Engine::Core::NodeTypeId{4}, {280.0F, -80.0F});
}

Engine::Core::DiagramModel& DiagramBasics::model() { return m_model; }
const Engine::Core::DiagramModel& DiagramBasics::model() const { return m_model; }
GridSettings& DiagramBasics::gridSettings() { return m_grid; }
const GridSettings& DiagramBasics::gridSettings() const { return m_grid; }
std::vector<Engine::Core::CanvasNodeId>& DiagramBasics::nodeIds() { return m_nodeIds; }
const std::vector<Engine::Core::CanvasNodeId>& DiagramBasics::nodeIds() const { return m_nodeIds; }
std::vector<Engine::Core::CanvasEdgeId>& DiagramBasics::edgeIds() { return m_edgeIds; }
const std::vector<Engine::Core::CanvasEdgeId>& DiagramBasics::edgeIds() const { return m_edgeIds; }

Engine::Core::CanvasNodeId DiagramBasics::createNode(Engine::Core::NodeTypeId typeId, Engine::Core::Vec2 position)
{
    const Engine::Core::CanvasNodeId id = m_model.createNode(typeId);
    if (Engine::Core::Node* node = m_model.getNode(id); node != nullptr)
    {
        if (m_grid.snapEnabled)
        {
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

Engine::Core::CanvasEdgeId DiagramBasics::connect(Engine::Core::CanvasConnectorId a, Engine::Core::CanvasConnectorId b)
{
    const Engine::Core::CanvasEdgeId edgeId = m_model.connect(a, b);
    if (edgeId.isValid())
    {
        m_edgeIds.push_back(edgeId);
    }
    return edgeId;
}

void DiagramBasics::deleteNode(Engine::Core::CanvasNodeId nodeId)
{
    m_model.removeNode(nodeId);
    m_nodeIds.erase(std::remove(m_nodeIds.begin(), m_nodeIds.end(), nodeId), m_nodeIds.end());
    m_edgeIds.erase(std::remove_if(m_edgeIds.begin(), m_edgeIds.end(), [this](Engine::Core::CanvasEdgeId id) {
                        return m_model.getEdge(id) == nullptr;
                    }),
                    m_edgeIds.end());
}

std::vector<Engine::Routing::EdgeRoute> DiagramBasics::routeAllEdges() const
{
    Engine::Routing::EdgeRouter router;
    std::vector<Engine::Routing::EdgeRoute> routes = router.routeAll(m_model);
    if (routes.empty())
    {
        routes.reserve(m_edgeIds.size());
        for (Engine::Core::CanvasEdgeId id : m_edgeIds)
        {
            const Engine::Core::Edge* edge = m_model.getEdge(id);
            if (edge == nullptr)
            {
                continue;
            }
            const Engine::Core::Connector* c0 = m_model.getConnector(edge->fromConnector);
            const Engine::Core::Connector* c1 = m_model.getConnector(edge->toConnector);
            if (c0 == nullptr || c1 == nullptr)
            {
                continue;
            }
            const Engine::Core::Node* n0 = m_model.getNode(c0->nodeId);
            const Engine::Core::Node* n1 = m_model.getNode(c1->nodeId);
            if (n0 == nullptr || n1 == nullptr)
            {
                continue;
            }
            Engine::Routing::EdgeRoute route{};
            route.edgeId = id;
            route.points.push_back({n0->position.x + n0->size.x, n0->position.y + n0->size.y * 0.5F});
            route.points.push_back({n1->position.x, n1->position.y + n1->size.y * 0.5F});
            routes.push_back(route);
        }
    }
    return routes;
}
} // namespace ScopeCanvas::Studio

#include <ScopeCanvas/demo/diagram/DiagramBasics.h>
#include <algorithm>
// #include <glm/geometric.hpp>

using namespace ScopeCanvas::Core;
using namespace ScopeCanvas::Core::Ids;

namespace ScopeCanvas::Demo {

DiagramBasics::DiagramBasics() {
    (void)createNode(NodeTypeId{10}, {-260.0F, -96.0F});
    (void)createNode(NodeTypeId{11}, {32.0F, -160.0F});
    (void)createNode(NodeTypeId{15}, {48.0F, 80.0F});
    (void)createNode(NodeTypeId{4}, {336.0F, -24.0F});

    if (m_nodeIds.size() >= 4U) {
        const Node* n0 = m_model.getNode(m_nodeIds[0]);
        const Node* n1 = m_model.getNode(m_nodeIds[1]);
        const Node* n2 = m_model.getNode(m_nodeIds[2]);
        const Node* n3 = m_model.getNode(m_nodeIds[3]);
        if (n0 != nullptr && n1 != nullptr && n2 != nullptr && n3 != nullptr) {
            connect(n0->connectors[1], n1->connectors[0]);
            connect(n0->connectors[1], n2->connectors[0]);
            connect(n2->connectors[1], n3->connectors[0]);
        }
    }
}

NodeId DiagramBasics::createNode(NodeTypeId typeId, glm::vec2 position) {
    const NodeId id = m_model.createNode(typeId);
    if (Node* node = m_model.getNode(id); node != nullptr) {
        if (m_grid.snapEnabled) {
            const float s = m_grid.cellSize;
            position.x = std::round(position.x / s) * s;
            position.y = std::round(position.y / s) * s;
        }
        node->position = position;
        node->size = {180.0F, 110.0F};
    }
    m_nodeIds.push_back(id);
    return id;
}

void DiagramBasics::deleteNode(NodeId nodeId) {
    m_model.removeNode(nodeId);
    m_nodeIds.erase(std::remove(m_nodeIds.begin(), m_nodeIds.end(), nodeId), m_nodeIds.end());
    m_selectedNodeIds.erase(std::remove(m_selectedNodeIds.begin(), m_selectedNodeIds.end(), nodeId),
                            m_selectedNodeIds.end());
    m_edgeIds.erase(std::remove_if(m_edgeIds.begin(), m_edgeIds.end(),
                                   [this](EdgeId id) { return m_model.getEdge(id) == nullptr; }),
                    m_edgeIds.end());
}

EdgeId DiagramBasics::connect(ConnectorId a, ConnectorId b) {
    const EdgeId edgeId = m_model.connect(a, b);
    if (edgeId.isValid())
        m_edgeIds.push_back(edgeId);

    return edgeId;
}

void DiagramBasics::deleteEdge(EdgeId edgeId) {
    if (!edgeId.isValid())
        return;

    m_model.disconnect(edgeId);
    m_edgeIds.erase(std::remove(m_edgeIds.begin(), m_edgeIds.end(), edgeId), m_edgeIds.end());
}

void DiagramBasics::setNodeSelected(NodeId nodeId, bool selected) {
    const auto it = std::find(m_selectedNodeIds.begin(), m_selectedNodeIds.end(), nodeId);
    if (selected) {
        if (it == m_selectedNodeIds.end())
            m_selectedNodeIds.push_back(nodeId);
    } else if (it != m_selectedNodeIds.end())
        m_selectedNodeIds.erase(it);
}

bool DiagramBasics::isNodeSelected(NodeId nodeId) const {
    return std::find(m_selectedNodeIds.begin(), m_selectedNodeIds.end(), nodeId) != m_selectedNodeIds.end();
}

bool DiagramBasics::canConnect(ConnectorId a, ConnectorId b) const {
    if (!a.isValid() || !b.isValid() || a == b)
        return false;

    const auto* connectorA = m_model.getConnector(a);
    const auto* connectorB = m_model.getConnector(b);
    if (connectorA == nullptr || connectorB == nullptr || connectorA->nodeId == connectorB->nodeId)
        return false;

    for (const auto edgeId : connectorA->edges) {
        const auto* edge = m_model.getEdge(edgeId);
        if (edge != nullptr && ((edge->fromConnector == a && edge->toConnector == b) ||
                                (edge->fromConnector == b && edge->toConnector == a)))
            return false;
    }
    return true;
}

} // namespace ScopeCanvas::Demo
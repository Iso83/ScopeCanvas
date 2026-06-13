#include <ScopeCanvas/widget/GraphDocument.h>
#include <algorithm>

namespace ScopeCanvas::Core {
using namespace Ids;
NodeId GraphDocument::createNode(NodeTypeId typeId) {
    const NodeId newNodeId = m_nodes.ids.createId();
    const ConnectorId firstConnectorId = m_connectors.ids.createId();
    const ConnectorId secondConnectorId = m_connectors.ids.createId();

    Node node{};
    node.id = newNodeId;
    node.typeId = typeId;
    node.connectors.push_back(firstConnectorId);
    node.connectors.push_back(secondConnectorId);
    m_nodes.instances.push_back(node);

    Connector firstConnector{};
    firstConnector.id = firstConnectorId;
    firstConnector.nodeId = newNodeId;
    m_connectors.instances.push_back(firstConnector);

    Connector secondConnector{};
    secondConnector.id = secondConnectorId;
    secondConnector.nodeId = newNodeId;
    m_connectors.instances.push_back(secondConnector);

    return newNodeId;
}

void GraphDocument::removeNode(NodeId nodeId) {
    Node* node = getNode(nodeId);
    if (node == nullptr) {
        return;
    }

    const std::vector<ConnectorId> connectors = node->connectors;
    for (ConnectorId connectorId : connectors) {
        Connector* connector = getConnector(connectorId);
        if (connector == nullptr)
            continue;

        const std::vector<EdgeId> edgeIds = connector->edges;
        for (EdgeId edgeId : edgeIds)
            disconnect(edgeId);

        m_connectors.erase(connectorId);
    }

    m_nodes.erase(nodeId);
}

EdgeId GraphDocument::connect(ConnectorId a, ConnectorId b) {
    Connector* fromConnector = getConnector(a);
    Connector* toConnector = getConnector(b);
    if (fromConnector == nullptr || toConnector == nullptr)
        return EdgeId{};

    const EdgeId edgeId = m_edges.ids.createId();
    m_edges.instances.push_back(Edge{edgeId, a, b});

    if (!contains(fromConnector->edges, edgeId))
        fromConnector->edges.push_back(edgeId);

    if (!contains(toConnector->edges, edgeId))
        toConnector->edges.push_back(edgeId);

    return edgeId;
}

void GraphDocument::disconnect(EdgeId edgeId) {
    Edge* edge = getEdge(edgeId);
    if (edge == nullptr) {
        return;
    }

    const ConnectorId fromId = edge->fromConnector;
    const ConnectorId toId = edge->toConnector;

    if (Connector* fromConnector = getConnector(fromId); fromConnector != nullptr)
        eraseValue(fromConnector->edges, edgeId);

    if (Connector* toConnector = getConnector(toId); toConnector != nullptr)
        eraseValue(toConnector->edges, edgeId);

    m_edges.erase(edgeId);
}

glm::vec2 GraphDocument::connectorWorld(const Node& node, std::size_t index) const {
    const bool output = (index % 2U) == 1U;
    const std::size_t sideIndex = index / 2U;
    const std::size_t sideCount = output ? node.connectors.size() / 2U : (node.connectors.size() + 1U) / 2U;
    constexpr float headerHeight = 24.0F;
    constexpr float verticalInset = 12.0F;
    const float bodyMinY = node.position.y + verticalInset;
    const float bodyMaxY = std::max(bodyMinY + 1.0F, node.position.y + node.size.y - headerHeight - verticalInset);
    const float bodyHeight = std::max(bodyMaxY - bodyMinY, 1.0F);
    const float step = bodyHeight / static_cast<float>(sideCount + 1U);
    const float y = bodyMinY + step * static_cast<float>(sideIndex + 1U);

    return {output ? node.position.x + node.size.x : node.position.x, y};
}

bool GraphDocument::contains(const std::vector<EdgeId>& values, EdgeId value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

void GraphDocument::eraseValue(std::vector<EdgeId>& values, EdgeId value) {
    values.erase(std::remove(values.begin(), values.end(), value), values.end());
}

} // namespace ScopeCanvas::Core

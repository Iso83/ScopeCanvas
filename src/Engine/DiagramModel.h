#pragma once

#include <glm/vec2.hpp>

#include <cstdint>
#include <vector>

enum class ConnectorSide {
    Top,
    Right,
    Bottom,
    Left,
};

struct Connector {
    uint32_t id;
    uint32_t nodeId;
    ConnectorSide side;
    float offset;
};

struct Edge {
    uint32_t id;
    uint32_t fromNode;
    uint32_t fromConnector;
    uint32_t toNode;
    uint32_t toConnector;
};

struct Node {
    uint32_t id;
    glm::vec2 position;
    glm::vec2 size;
    bool selected;
    std::vector<Connector> connectors;
};

glm::vec2 connectorWorldPosition(const Node& node, const Connector& connector);
std::vector<Connector> createDefaultConnectors(uint32_t nodeId, uint32_t& nextConnectorId);

class DiagramModel {
public:
    DiagramModel() = default;

    std::vector<Node>& nodes() { return m_nodes; }
    const std::vector<Node>& nodes() const { return m_nodes; }

    std::vector<Edge>& edges() { return m_edges; }
    const std::vector<Edge>& edges() const { return m_edges; }

    void addEdge(const Edge& edge);
    bool removeEdge(uint32_t edgeId);

    Node* findNode(uint32_t nodeId);
    const Node* findNode(uint32_t nodeId) const;

    Connector* findConnector(uint32_t nodeId, uint32_t connectorId);
    const Connector* findConnector(uint32_t nodeId, uint32_t connectorId) const;

private:
    std::vector<Node> m_nodes;
    std::vector<Edge> m_edges;
};

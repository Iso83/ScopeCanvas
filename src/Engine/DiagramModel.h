#pragma once

#include <glm/vec2.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct NodeType;
class NodeTypeRegistry;

enum class ConnectorSide {
    Top,
    Right,
    Bottom,
    Left,
};

enum class ConnectorDirection {
    Input,
    Output,
};

struct Connector {
    uint32_t id;
    uint32_t nodeId;
    ConnectorSide side;
    float offset;
    ConnectorDirection direction = ConnectorDirection::Input;
};

struct ConnectorTemplate {
    ConnectorSide side;
    float offset;
    ConnectorDirection direction;
};

struct Edge {
    uint32_t id;
    uint32_t fromNode;
    uint32_t fromConnector;
    uint32_t toNode;
    uint32_t toConnector;
    bool selected = false;
};

struct Node {
    uint32_t id;
    std::string nodeTypeId;
    std::string title;
    glm::vec2 position;
    glm::vec2 size;
    bool selected;
    std::vector<Connector> connectors;
};

glm::vec2 connectorWorldPosition(const Node& node, const Connector& connector);
std::vector<Connector> createDefaultConnectors(uint32_t nodeId, uint32_t& nextConnectorId);

class DiagramModel {
public:
    DiagramModel();
    ~DiagramModel();

    DiagramModel(const DiagramModel&) = delete;
    DiagramModel& operator=(const DiagramModel&) = delete;
    DiagramModel(DiagramModel&&) = delete;
    DiagramModel& operator=(DiagramModel&&) = delete;

    std::vector<Node>& nodes() { return m_nodes; }
    const std::vector<Node>& nodes() const { return m_nodes; }

    std::vector<Edge>& edges() { return m_edges; }
    const std::vector<Edge>& edges() const { return m_edges; }

    Node* createNode(const glm::vec2& position, const glm::vec2& size = {200.0f, 120.0f});
    Node* createNodeOfType(const std::string& nodeTypeId,
                           const glm::vec2& position,
                           const glm::vec2& size = {200.0f, 120.0f});
    Node* createNodeWithConnectors(const glm::vec2& position,
                                   const glm::vec2& size,
                                   const std::vector<ConnectorTemplate>& connectors,
                                   const std::string& nodeTypeId = "Custom",
                                   const std::string& title = "Node");
    Node* addNode(const Node& node);
    Node* duplicateNode(uint32_t nodeId, const glm::vec2& offset = {40.0f, 40.0f});
    bool removeNode(uint32_t nodeId);
    size_t removeSelectedNodes();

    void clear();
    void clearNodeSelection();
    bool addEdge(const Edge& edge);
    bool createEdge(uint32_t fromNode, uint32_t fromConnector, uint32_t toNode, uint32_t toConnector);
    bool removeEdge(uint32_t edgeId);

    void syncIdCounters();

    Node* findNode(uint32_t nodeId);
    const Node* findNode(uint32_t nodeId) const;

    Connector* findConnector(uint32_t nodeId, uint32_t connectorId);
    const Connector* findConnector(uint32_t nodeId, uint32_t connectorId) const;

    Edge* findEdge(uint32_t edgeId);
    const Edge* findEdge(uint32_t edgeId) const;

    const NodeTypeRegistry& nodeTypeRegistry() const;

private:
    bool removeEdgesForNode(uint32_t nodeId);

    uint32_t m_nextNodeId = 1;
    uint32_t m_nextConnectorId = 1;
    uint32_t m_nextEdgeId = 1;
    std::vector<Node> m_nodes;
    std::vector<Edge> m_edges;
    std::unique_ptr<NodeTypeRegistry> m_nodeTypeRegistry;
};

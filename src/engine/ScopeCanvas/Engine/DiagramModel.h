#pragma once

#include <glm/vec2.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Engine/ConnectorDirection.h"
#include "Engine/CanvasIds.h"

struct NodeType;
class NodeTypeRegistry;
class DiagramModel;

enum class ConnectorSide {
	Top,
	Right,
	Bottom,
	Left,
};

struct Connector {
	CanvasConnectorId id;
	CanvasNodeId nodeId;
	ConnectorSide side;
	float offset;
	ConnectorDirection direction = ConnectorDirection::Input;
	int maxConnections = 1;
	int semanticSlot = 0;
	std::string dataKind;
};

struct ConnectorTemplate {
	ConnectorSide side;
	float offset;
	ConnectorDirection direction;
	int maxConnections = 1;
};

struct EdgeRoute {
	std::vector<glm::vec2> points;
};

struct Edge {
	CanvasEdgeId id;
	CanvasNodeId fromNode;
	CanvasConnectorId fromConnector;
	CanvasNodeId toNode;
	CanvasConnectorId toConnector;
	bool selected = false;
	EdgeRoute route;
};

struct Node {
	CanvasNodeId id;
	std::string nodeTypeId;
	std::string title;
	glm::vec2 position;
	glm::vec2 size;
	bool selected = false;
	bool collapsed = false;
	bool allowChildren = false;
	CanvasNodeId parentId{};
	std::vector<CanvasNodeId> children;
	uint64_t externalRef = 0;
	std::vector<Connector> connectors;
};

struct GraphView {
	CanvasViewId id;
	glm::vec2 cameraPosition;
	float zoom = 1.0f;
	CanvasNodeId focusNode{};
};

glm::vec2 connectorWorldPosition(const Node &node, const Connector &connector);
std::vector<Connector> createDefaultConnectors(CanvasNodeId nodeId, uint32_t &nextConnectorId);

class IConnectionRule {
public:
	virtual ~IConnectionRule() = default;
	virtual bool canConnect(const DiagramModel &model,
		const Connector &from,
		const Connector &to,
		std::string *reason) const = 0;
};

class DiagramModel {
public:
	DiagramModel();
	~DiagramModel();

	DiagramModel(const DiagramModel &) = delete;
	DiagramModel &operator=(const DiagramModel &) = delete;
	DiagramModel(DiagramModel &&) = delete;
	DiagramModel &operator=(DiagramModel &&) = delete;

	std::vector<Node> &nodes() { return m_nodes; }
	const std::vector<Node> &nodes() const { return m_nodes; }

	std::vector<Edge> &edges() { return m_edges; }
	const std::vector<Edge> &edges() const { return m_edges; }

	Node *createNode(const glm::vec2 &position, const glm::vec2 &size = { 200.0f, 120.0f });
	Node *createNodeOfType(const std::string &nodeTypeId,
		const glm::vec2 &position,
		const glm::vec2 &size = { 200.0f, 120.0f });
	Node *createNodeWithConnectors(const glm::vec2 &position,
		const glm::vec2 &size,
		const std::vector<ConnectorTemplate> &connectors,
		const std::string &nodeTypeId = "Custom",
		const std::string &title = "Node");
	Node *addNode(const Node &node);
	Node *duplicateNode(CanvasNodeId nodeId, const glm::vec2 &offset = { 40.0f, 40.0f });
	bool removeNode(CanvasNodeId nodeId);
	size_t removeSelectedNodes();

	void clear();
	void clearNodeSelection();
	bool isValidConnection(CanvasNodeId fromNode,
		CanvasConnectorId fromConnector,
		CanvasNodeId toNode,
		CanvasConnectorId toConnector,
		std::string *reason = nullptr) const;
	bool addEdge(const Edge &edge);
	bool createEdge(CanvasNodeId fromNode, CanvasConnectorId fromConnector, CanvasNodeId toNode, CanvasConnectorId toConnector);
	bool removeEdge(CanvasEdgeId edgeId);
	void recomputeRoutesForNode(CanvasNodeId nodeId);

	bool addChildNode(CanvasNodeId parentId, CanvasNodeId childId);
	bool detachFromParent(CanvasNodeId nodeId);
	void setNodeCollapsed(CanvasNodeId nodeId, bool collapsed);
	bool isNodeHiddenByCollapsedAncestor(CanvasNodeId nodeId) const;

	void syncIdCounters();

	Node *findNode(CanvasNodeId nodeId);
	const Node *findNode(CanvasNodeId nodeId) const;

	Connector *findConnector(CanvasNodeId nodeId, CanvasConnectorId connectorId);
	const Connector *findConnector(CanvasNodeId nodeId, CanvasConnectorId connectorId) const;

	Edge *findEdge(CanvasEdgeId edgeId);
	const Edge *findEdge(CanvasEdgeId edgeId) const;

	void setConnectionRule(std::shared_ptr<IConnectionRule> rule);

	const NodeTypeRegistry &nodeTypeRegistry() const;

private:
	bool removeEdgesForNode(CanvasNodeId nodeId);

	uint32_t m_nextNodeId = 1;
	uint32_t m_nextConnectorId = 1;
	uint32_t m_nextEdgeId = 1;
	std::vector<Node> m_nodes;
	std::vector<Edge> m_edges;
	std::unique_ptr<NodeTypeRegistry> m_nodeTypeRegistry;
	std::shared_ptr<IConnectionRule> m_connectionRule;
};

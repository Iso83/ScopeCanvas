#include "Interaction/ConnectController.h"

#include "Interaction/EdgeInteractionController.h"

#include <iostream>


namespace {
	bool tryBuildDirectedConnection(const DiagramModel &model,
		CanvasNodeId startNodeId,
		CanvasConnectorId startConnectorId,
		CanvasNodeId endNodeId,
		CanvasConnectorId endConnectorId,
		CanvasNodeId &outFromNode,
		CanvasConnectorId &outFromConnector,
		CanvasNodeId &outToNode,
		CanvasConnectorId &outToConnector) {
		const Connector *startConnector = model.findConnector(startNodeId, startConnectorId);
		const Connector *endConnector = model.findConnector(endNodeId, endConnectorId);
		if (startConnector == nullptr || endConnector == nullptr) {
			return false;
		}

		if (startConnector->direction == ConnectorDirection::Output &&
			endConnector->direction == ConnectorDirection::Input) {
			outFromNode = startNodeId;
			outFromConnector = startConnectorId;
			outToNode = endNodeId;
			outToConnector = endConnectorId;
			return true;
		}

		if (startConnector->direction == ConnectorDirection::Input &&
			endConnector->direction == ConnectorDirection::Output) {
			outFromNode = endNodeId;
			outFromConnector = endConnectorId;
			outToNode = startNodeId;
			outToConnector = startConnectorId;
			return true;
		}

		return false;
	}

	int connectionCountFor(const DiagramModel &model, CanvasNodeId nodeId, CanvasConnectorId connectorId) {
		int currentConnections = 0;
		for (const Edge &edge : model.edges()) {
			if ((edge.fromNode == nodeId && edge.fromConnector == connectorId) ||
				(edge.toNode == nodeId && edge.toConnector == connectorId)) {
				++currentConnections;
			}
		}

		return currentConnections;
	}

	CanvasEdgeId findEdgeToReplace(const DiagramModel &model,
		CanvasNodeId nodeId,
		CanvasConnectorId connectorId,
		CanvasEdgeId excludedEdgeId) {
		for (const Edge &edge : model.edges()) {
			const bool usesConnector =
				(edge.fromNode == nodeId && edge.fromConnector == connectorId) ||
				(edge.toNode == nodeId && edge.toConnector == connectorId);
			if (usesConnector && edge.id != excludedEdgeId) {
				return edge.id;
			}
		}

		return CanvasEdgeId{};
	}
}

bool ConnectController::onMouseDown(const DiagramModel &model,
	const glm::vec2 &mouseWorld,
	float zoom,
	bool overrideMode) {
	CanvasNodeId startNodeId{};
	const Connector *connector = hitTestConnector(model, mouseWorld, zoom, startNodeId);
	if (connector == nullptr) {
		return false;
	}

	if (!overrideMode && connectorHasEdge(model, startNodeId, connector->id)) {
		return false;
	}

	m_connecting = true;
	m_startNodeId = startNodeId;
	m_startConnectorId = connector->id;
	m_previewPosition = mouseWorld;
	return true;
}

void ConnectController::beginReconnect(CanvasEdgeId edgeId,
	bool startEndpoint,
	CanvasNodeId fixedNodeId,
	CanvasConnectorId fixedConnectorId,
	const glm::vec2 &mouseWorld) {
	m_connecting = true;
	m_reconnectActive = true;
	m_reconnectEdgeId = edgeId;
	m_reconnectStartEndpoint = startEndpoint;
	m_startNodeId = fixedNodeId;
	m_startConnectorId = fixedConnectorId;
	m_previewPosition = mouseWorld;
}

void ConnectController::onMouseMove(const glm::vec2 &mouseWorld) {
	if (!m_connecting) {
		return;
	}

	m_previewPosition = mouseWorld;
}

ConnectController::ConnectionResult ConnectController::onMouseUp(const DiagramModel &model,
	const glm::vec2 &mouseWorld,
	float zoom) {
	if (!m_connecting) {
		return {};
	}

	m_previewPosition = mouseWorld;

	CanvasNodeId endNodeId{};
	const Connector *endConnector = hitTestConnector(model, mouseWorld, zoom, endNodeId);
	if (endConnector == nullptr) {
		std::cout << "Connection cancelled\n";
		reset();
		return { .handled = true };
	}

	const bool sameEndpoint = endNodeId == m_startNodeId && endConnector->id == m_startConnectorId;
	const bool sameNodeConnection = endNodeId == m_startNodeId;

	bool duplicateEdge = false;
	if (!sameEndpoint) {
		for (const Edge &edge : model.edges()) {
			const bool sameDirection =
				edge.fromNode == m_startNodeId &&
				edge.fromConnector == m_startConnectorId &&
				edge.toNode == endNodeId &&
				edge.toConnector == endConnector->id;
			const bool reverseDirection =
				edge.fromNode == endNodeId &&
				edge.fromConnector == endConnector->id &&
				edge.toNode == m_startNodeId &&
				edge.toConnector == m_startConnectorId;
			if (sameDirection || reverseDirection) {
				duplicateEdge = true;
				break;
			}
		}
	}

	if (sameEndpoint || sameNodeConnection || duplicateEdge) {
		std::cout << "Connection cancelled\n";
		reset();
		return { .handled = true };
	}

	CanvasNodeId fromNode{};
	CanvasConnectorId fromConnector{};
	CanvasNodeId toNode{};
	CanvasConnectorId toConnector{};

	if (!tryBuildDirectedConnection(model,
		m_startNodeId,
		m_startConnectorId,
		endNodeId,
		endConnector->id,
		fromNode,
		fromConnector,
		toNode,
		toConnector)) {
		std::cout << "Connection cancelled\n";
		reset();
		return { .handled = true };
	}

	if (m_reconnectActive && m_reconnectStartEndpoint) {
		if (!tryBuildDirectedConnection(model,
			endNodeId,
			endConnector->id,
			m_startNodeId,
			m_startConnectorId,
			fromNode,
			fromConnector,
			toNode,
			toConnector)) {
			std::cout << "Connection cancelled\n";
			reset();
			return { .handled = true };
		}
	}

	ConnectionResult result;
	result.handled = true;

	if (!model.isValidConnection(fromNode, fromConnector, toNode, toConnector)) {
		if (!m_reconnectActive) {
			std::cout << "Connection cancelled\n";
			reset();
			return { .handled = true };
		}

		const Connector *toConnectorPtr = model.findConnector(toNode, toConnector);
		if (toConnectorPtr == nullptr) {
			std::cout << "Connection cancelled\n";
			reset();
			return { .handled = true };
		}

		const int toConnectionCount = connectionCountFor(model, toNode, toConnector);
		if (toConnectionCount < toConnectorPtr->maxConnections) {
			std::cout << "Connection cancelled\n";
			reset();
			return { .handled = true };
		}

		const CanvasEdgeId edgeToReplace = findEdgeToReplace(model, toNode, toConnector, m_reconnectEdgeId);
		if (!edgeToReplace) {
			std::cout << "Connection cancelled\n";
			reset();
			return { .handled = true };
		}

		result.removeEdge = true;
		result.edgeToRemoveId = edgeToReplace;
	}

	result.createEdge = true;
	result.edge = Edge{ allocateEdgeId(model), fromNode, fromConnector, toNode, toConnector, false };

	reset();
	return result;
}

const Connector *ConnectController::hitTestConnector(const DiagramModel &model,
	const glm::vec2 &mouseWorld,
	float zoom,
	CanvasNodeId &nodeId) const {
	nodeId = CanvasNodeId{};
	return EdgeInteractionController::hitTestConnector(model, mouseWorld, zoom, &nodeId);
}

bool ConnectController::connectorHasEdge(const DiagramModel &model, CanvasNodeId nodeId, CanvasConnectorId connectorId) {
	for (const Edge &edge : model.edges()) {
		if ((edge.fromNode == nodeId && edge.fromConnector == connectorId) ||
			(edge.toNode == nodeId && edge.toConnector == connectorId)) {
			return true;
		}
	}

	return false;
}

CanvasEdgeId ConnectController::allocateEdgeId(const DiagramModel &model) {
	uint32_t maxId = 0;
	for (const Edge &edge : model.edges()) {
		if (edge.id.value > maxId) {
			maxId = edge.id.value;
		}
	}

	return CanvasEdgeId{ maxId + 1 };
}

void ConnectController::reset() {
	m_connecting = false;
	m_startNodeId = CanvasNodeId{};
	m_startConnectorId = CanvasConnectorId{};
	m_reconnectEdgeId = CanvasEdgeId{};
	m_reconnectActive = false;
	m_reconnectStartEndpoint = false;
	m_previewPosition = glm::vec2(0.0f);
}

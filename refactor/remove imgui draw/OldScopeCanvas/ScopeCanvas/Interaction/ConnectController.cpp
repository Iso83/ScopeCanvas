#include "Interaction/ConnectController.h"

#include "Interaction/EdgeInteractionController.h"

#include <iostream>


namespace {
	bool tryBuildDirectedConnection(const DiagramModel &model,
		uint32_t startNodeId,
		uint32_t startConnectorId,
		uint32_t endNodeId,
		uint32_t endConnectorId,
		uint32_t &outFromNode,
		uint32_t &outFromConnector,
		uint32_t &outToNode,
		uint32_t &outToConnector) {
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

	int connectionCountFor(const DiagramModel &model, uint32_t nodeId, uint32_t connectorId) {
		int currentConnections = 0;
		for (const Edge &edge : model.edges()) {
			if ((edge.fromNode == nodeId && edge.fromConnector == connectorId) ||
				(edge.toNode == nodeId && edge.toConnector == connectorId)) {
				++currentConnections;
			}
		}

		return currentConnections;
	}

	uint32_t findEdgeToReplace(const DiagramModel &model,
		uint32_t nodeId,
		uint32_t connectorId,
		uint32_t excludedEdgeId) {
		for (const Edge &edge : model.edges()) {
			const bool usesConnector =
				(edge.fromNode == nodeId && edge.fromConnector == connectorId) ||
				(edge.toNode == nodeId && edge.toConnector == connectorId);
			if (usesConnector && edge.id != excludedEdgeId) {
				return edge.id;
			}
		}

		return 0;
	}
}

bool ConnectController::onMouseDown(const DiagramModel &model,
	const glm::vec2 &mouseWorld,
	float zoom,
	bool overrideMode) {
	uint32_t startNodeId = 0;
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

void ConnectController::beginReconnect(uint32_t edgeId,
	bool startEndpoint,
	uint32_t fixedNodeId,
	uint32_t fixedConnectorId,
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

	uint32_t endNodeId = 0;
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

	uint32_t fromNode = 0;
	uint32_t fromConnector = 0;
	uint32_t toNode = 0;
	uint32_t toConnector = 0;

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

		const uint32_t edgeToReplace = findEdgeToReplace(model, toNode, toConnector, m_reconnectEdgeId);
		if (edgeToReplace == 0) {
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
	uint32_t &nodeId) const {
	nodeId = 0;
	return EdgeInteractionController::hitTestConnector(model, mouseWorld, zoom, &nodeId);
}

bool ConnectController::connectorHasEdge(const DiagramModel &model, uint32_t nodeId, uint32_t connectorId) {
	for (const Edge &edge : model.edges()) {
		if ((edge.fromNode == nodeId && edge.fromConnector == connectorId) ||
			(edge.toNode == nodeId && edge.toConnector == connectorId)) {
			return true;
		}
	}

	return false;
}

uint32_t ConnectController::allocateEdgeId(const DiagramModel &model) {
	uint32_t maxId = 0;
	for (const Edge &edge : model.edges()) {
		if (edge.id > maxId) {
			maxId = edge.id;
		}
	}

	return maxId + 1;
}

void ConnectController::reset() {
	m_connecting = false;
	m_startNodeId = 0;
	m_startConnectorId = 0;
	m_reconnectEdgeId = 0;
	m_reconnectActive = false;
	m_reconnectStartEndpoint = false;
	m_previewPosition = glm::vec2(0.0f);
}

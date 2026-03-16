#pragma once

#include "Engine/DiagramModel.h"

#include <glm/vec2.hpp>

class ConnectController {
public:
	struct ConnectionResult {
		bool handled = false;
		bool createEdge = false;
		bool removeEdge = false;
		CanvasEdgeId edgeToRemoveId{};
		Edge edge{};
	};

	bool onMouseDown(const DiagramModel &model, const glm::vec2 &mouseWorld, float zoom, bool overrideMode);
	void onMouseMove(const glm::vec2 &mouseWorld);
	ConnectionResult onMouseUp(const DiagramModel &model, const glm::vec2 &mouseWorld, float zoom);

	void beginReconnect(CanvasEdgeId edgeId,
		bool startEndpoint,
		CanvasNodeId fixedNodeId,
		CanvasConnectorId fixedConnectorId,
		const glm::vec2 &mouseWorld);

	bool isConnecting() const { return m_connecting; }
	CanvasNodeId startNodeId() const { return m_startNodeId; }
	CanvasConnectorId startConnectorId() const { return m_startConnectorId; }
	const glm::vec2 &previewPosition() const { return m_previewPosition; }

private:
	const Connector *hitTestConnector(const DiagramModel &model,
		const glm::vec2 &mouseWorld,
		float zoom,
		CanvasNodeId &nodeId) const;

	static bool connectorHasEdge(const DiagramModel &model, CanvasNodeId nodeId, CanvasConnectorId connectorId);
	static CanvasEdgeId allocateEdgeId(const DiagramModel &model);

	void reset();

	bool m_connecting = false;
	CanvasNodeId m_startNodeId{};
	CanvasConnectorId m_startConnectorId{};
	CanvasEdgeId m_reconnectEdgeId{};
	bool m_reconnectActive = false;
	bool m_reconnectStartEndpoint = false;
	glm::vec2 m_previewPosition = glm::vec2(0.0f);
};

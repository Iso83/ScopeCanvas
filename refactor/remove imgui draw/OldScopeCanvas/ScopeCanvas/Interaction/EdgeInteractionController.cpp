#include "Interaction/EdgeInteractionController.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>

#include <cmath>

Edge *EdgeInteractionController::hitTestEdge(DiagramModel &model, const glm::vec2 &mouseWorld, float zoom) {
	const Edge *constModelEdge = hitTestEdge(static_cast<const DiagramModel &>(model), mouseWorld, zoom);
	if (constModelEdge == nullptr) {
		return nullptr;
	}

	for (Edge &edge : model.edges()) {
		if (edge.id == constModelEdge->id) {
			return &edge;
		}
	}

	return nullptr;
}

const Edge *EdgeInteractionController::hitTestEdge(const DiagramModel &model,
	const glm::vec2 &mouseWorld,
	float zoom) {
	const float threshold = kHitThresholdPixels / glm::max(zoom, 0.001f);
	std::vector<glm::vec2> sampledPoints;
	sampledPoints.reserve(static_cast<size_t>(kBezierSegments) + 1);

	for (const Edge &edge : model.edges()) {
		const Node *fromNode = model.findNode(edge.fromNode);
		const Node *toNode = model.findNode(edge.toNode);
		const Connector *fromConnector = model.findConnector(edge.fromNode, edge.fromConnector);
		const Connector *toConnector = model.findConnector(edge.toNode, edge.toConnector);

		if (fromNode == nullptr || toNode == nullptr || fromConnector == nullptr || toConnector == nullptr) {
			continue;
		}

		const glm::vec2 p0 = connectorWorldPosition(*fromNode, *fromConnector);
		const glm::vec2 p3 = connectorWorldPosition(*toNode, *toConnector);

		glm::vec2 p1(0.0f);
		glm::vec2 p2(0.0f);
		computeBezierControls(p0, p3, p1, p2);

		sampledPoints.clear();
		appendBezierSamples(sampledPoints, p0, p1, p2, p3);

		for (size_t i = 1; i < sampledPoints.size(); ++i) {
			const float distance = distanceToSegment(mouseWorld, sampledPoints[i - 1], sampledPoints[i]);
			if (distance < threshold) {
				return &edge;
			}
		}
	}

	return nullptr;
}

const Connector *EdgeInteractionController::hitTestConnector(const DiagramModel &model,
	const glm::vec2 &mouseWorld,
	float zoom,
	uint32_t *outNodeId) {
	const float threshold = kHitThresholdPixels / glm::max(zoom, 0.001f);

	for (auto nodeIt = model.nodes().rbegin(); nodeIt != model.nodes().rend(); ++nodeIt) {
		const Node &node = *nodeIt;
		for (const Connector &connector : node.connectors) {
			const glm::vec2 connectorPos = connectorWorldPosition(node, connector);
			if (glm::distance(mouseWorld, connectorPos) < threshold) {
				if (outNodeId != nullptr) {
					*outNodeId = node.id;
				}
				return &connector;
			}
		}
	}

	return nullptr;
}

EdgeEndpointHit EdgeInteractionController::hitTestEdgeEndpoint(const DiagramModel &model,
	const glm::vec2 &mouseWorld,
	float zoom) {
	const float threshold = kHitThresholdPixels / glm::max(zoom, 0.001f);

	for (const Edge &edge : model.edges()) {
		const Node *fromNode = model.findNode(edge.fromNode);
		const Node *toNode = model.findNode(edge.toNode);
		const Connector *fromConnector = model.findConnector(edge.fromNode, edge.fromConnector);
		const Connector *toConnector = model.findConnector(edge.toNode, edge.toConnector);
		if (fromNode == nullptr || toNode == nullptr || fromConnector == nullptr || toConnector == nullptr) {
			continue;
		}

		const glm::vec2 startPosition = connectorWorldPosition(*fromNode, *fromConnector);
		if (glm::distance(mouseWorld, startPosition) < threshold) {
			return EdgeEndpointHit{ edge.id, true, true };
		}

		const glm::vec2 endPosition = connectorWorldPosition(*toNode, *toConnector);
		if (glm::distance(mouseWorld, endPosition) < threshold) {
			return EdgeEndpointHit{ edge.id, true, false };
		}
	}

	return EdgeEndpointHit{};
}

void EdgeInteractionController::appendBezierSamples(std::vector<glm::vec2> &points,
	const glm::vec2 &p0,
	const glm::vec2 &p1,
	const glm::vec2 &p2,
	const glm::vec2 &p3) {
	points.reserve(static_cast<size_t>(kBezierSegments) + 1);

	for (int i = 0; i <= kBezierSegments; ++i) {
		const float t = static_cast<float>(i) / static_cast<float>(kBezierSegments);
		const float omt = 1.0f - t;
		const glm::vec2 point =
			omt * omt * omt * p0 +
			3.0f * omt * omt * t * p1 +
			3.0f * omt * t * t * p2 +
			t * t * t * p3;
		points.push_back(point);
	}
}

void EdgeInteractionController::computeBezierControls(const glm::vec2 &p0,
	const glm::vec2 &p3,
	glm::vec2 &p1,
	glm::vec2 &p2) {
	const float dx = std::abs(p3.x - p0.x);
	const float controlDistance = glm::max(80.0f, dx * 0.5f);
	p1 = p0 + glm::vec2(controlDistance, 0.0f);
	p2 = p3 - glm::vec2(controlDistance, 0.0f);
}

float EdgeInteractionController::distanceToSegment(const glm::vec2 &point,
	const glm::vec2 &segmentStart,
	const glm::vec2 &segmentEnd) {
	const glm::vec2 segment = segmentEnd - segmentStart;
	const float lengthSquared = glm::dot(segment, segment);

	if (lengthSquared <= 0.000001f) {
		return glm::distance(point, segmentStart);
	}

	const float t = glm::clamp(glm::dot(point - segmentStart, segment) / lengthSquared, 0.0f, 1.0f);
	const glm::vec2 projected = segmentStart + segment * t;
	return glm::distance(point, projected);
}

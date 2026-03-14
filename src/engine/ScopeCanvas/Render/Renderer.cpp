#include "Render/Renderer.h"

#include <glad/glad.h>

#include <algorithm>
#include <limits>
#include <unordered_set>
#include <vector>

Renderer::Renderer() : m_viewportWidth(1280), m_viewportHeight(720) {}

bool Renderer::init(
	int viewportWidth,
	int viewportHeight) {
	m_viewportWidth = viewportWidth;
	m_viewportHeight = viewportHeight;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_camera.setViewportSize(viewportWidth, viewportHeight);
	return (m_gridRenderer.init() &&
		m_edgeRenderer.init() &&
		m_nodeRenderer.init() &&
		m_selectionRectRenderer.init());
}

void Renderer::resize(int width, int height) {
	m_viewportWidth = width;
	m_viewportHeight = height;
	m_camera.setViewportSize(width, height);
	glViewport(0, 0, width, height);
}

void Renderer::render(const GraphDocument &model,
	const GraphView &view,
	const Viewport &viewport,
	bool gridEnabled,
	float gridCellSize,
	bool renderEdges) {
	glClearColor(0.10f, 0.11f, 0.12f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glm::vec2 cameraPosition = view.cameraPosition;
	if (view.focusNode != 0) {
		const Node *focusNode = model.findNode(view.focusNode);
		if (focusNode != nullptr) {
			cameraPosition = focusNode->position + (focusNode->size * 0.5f);
		}
	}

	m_camera.setPosition(cameraPosition);
	m_camera.setZoom(view.zoom);
	const glm::mat4 viewProjection = m_camera.viewProjection();

	std::unordered_set<uint32_t> hiddenNodeIds;
	std::vector<Node> groupNodes;

	for (const Group &group : model.groups()) {
		if (!group.collapsed) {
			continue;
		}

		float minX = std::numeric_limits<float>::max();
		float minY = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		float maxY = std::numeric_limits<float>::lowest();
		bool hasChildNodes = false;

		for (uint32_t childNodeId : group.children) {
			const Node *childNode = model.findNode(childNodeId);
			if (childNode == nullptr) {
				continue;
			}

			hiddenNodeIds.insert(childNodeId);
			minX = std::min(minX, childNode->position.x);
			minY = std::min(minY, childNode->position.y);
			maxX = std::max(maxX, childNode->position.x + childNode->size.x);
			maxY = std::max(maxY, childNode->position.y + childNode->size.y);
			hasChildNodes = true;
		}

		if (!hasChildNodes) {
			continue;
		}

		constexpr float kPadding = 24.0f;
		Node groupNode{};
		groupNode.id = group.id;
		groupNode.nodeTypeId = "Group";
		groupNode.title = "Group";
		groupNode.position = { minX - kPadding, minY - kPadding };
		groupNode.size = { (maxX - minX) + 2.0f * kPadding, (maxY - minY) + 2.0f * kPadding };
		groupNode.selected = false;
		groupNode.groupId = 0;
		groupNodes.push_back(groupNode);
	}

	std::vector<Node> visibleNodes;
	visibleNodes.reserve(model.nodes().size() + groupNodes.size());
	for (const Node &node : model.nodes()) {
		if (hiddenNodeIds.find(node.id) == hiddenNodeIds.end()) {
			visibleNodes.push_back(node);
		}
	}

	visibleNodes.insert(visibleNodes.end(), groupNodes.begin(), groupNodes.end());

	if (gridEnabled) {
		m_gridRenderer.render(viewProjection, m_viewportWidth, m_viewportHeight, gridCellSize);
	}
	if (renderEdges) {
		m_edgeRenderer.renderEdges(model, viewProjection, viewport.hoveredEdgeId);
	}
	m_nodeRenderer.render(visibleNodes, viewProjection);
	m_edgeRenderer.renderConnectors(visibleNodes, viewProjection, viewport.hoveredConnectorId);

	if (viewport.selectionRectActive) {
		m_selectionRectRenderer.render(viewProjection, viewport.selectionRectStart, viewport.selectionRectEnd);
	}

	if (viewport.previewActive) {
		m_edgeRenderer.renderPreviewEdge(model,
			viewport.previewStartNode,
			viewport.previewStartConnector,
			viewport.previewPosition,
			viewProjection);
	}
}

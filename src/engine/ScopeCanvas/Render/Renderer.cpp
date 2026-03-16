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
	bool renderEdges,
	bool renderConnectors) {
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

	std::vector<Node> visibleNodes;
	visibleNodes.reserve(model.nodes().size());
	const glm::vec2 halfVisible(
		static_cast<float>(m_viewportWidth) / (2.0f * std::max(0.0001f, view.zoom)),
		static_cast<float>(m_viewportHeight) / (2.0f * std::max(0.0001f, view.zoom)));
	const glm::vec2 visibleMin = cameraPosition - halfVisible;
	const glm::vec2 visibleMax = cameraPosition + halfVisible;

	auto intersectsVisible = [&](const Node &node) {
		const glm::vec2 nodeMin = node.position;
		const glm::vec2 nodeMax = node.position + node.size;
		return !(nodeMax.x < visibleMin.x || nodeMin.x > visibleMax.x ||
			nodeMax.y < visibleMin.y || nodeMin.y > visibleMax.y);
	};

	for (const Node &node : model.nodes()) {
		if (!model.isNodeHiddenByCollapsedAncestor(node.id) && intersectsVisible(node)) {
			visibleNodes.push_back(node);
		}
	}

	if (gridEnabled) {
		m_gridRenderer.render(viewProjection, m_viewportWidth, m_viewportHeight, gridCellSize);
	}
	if (renderEdges) {
		m_edgeRenderer.renderEdges(model, viewProjection, viewport.hoveredEdgeId);
	}
	m_nodeRenderer.render(visibleNodes, viewProjection);
	if (renderConnectors) {
		m_edgeRenderer.renderConnectors(visibleNodes, viewProjection, viewport.hoveredConnectorId);
	}

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

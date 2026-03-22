#pragma once

#include <glm/vec2.hpp>

#include <cstdint>

struct Viewport {
	uint32_t hoveredEdgeId = 0;
	uint32_t hoveredConnectorId = 0;

	bool selectionRectActive = false;
	glm::vec2 selectionRectStart = glm::vec2(0.0f);
	glm::vec2 selectionRectEnd = glm::vec2(0.0f);

	bool previewActive = false;
	uint32_t previewStartNode = 0;
	uint32_t previewStartConnector = 0;
	glm::vec2 previewPosition = glm::vec2(0.0f);
};

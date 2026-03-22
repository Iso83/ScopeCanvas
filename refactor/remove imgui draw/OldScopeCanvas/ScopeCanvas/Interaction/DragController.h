#pragma once

#include "Engine/DiagramModel.h"
#include "Interaction/Commands/GraphCommands.h"

#include <glm/vec2.hpp>

#include <vector>

class DragController {
public:
	void onMouseDown(DiagramModel &model, const glm::vec2 &mouseWorld);
	std::vector<MoveNodesCommand::MoveItem> onMouseUp(DiagramModel &model);
	void update(DiagramModel &model, const glm::vec2 &mouseWorld);
	bool isDragging() const { return m_dragging; }

private:
	static Node *hitTestNode(DiagramModel &model, const glm::vec2 &mouseWorld);

	struct DragItem {
		uint32_t nodeId = 0;
		glm::vec2 startPosition = glm::vec2(0.0f);
	};

	void reset();

	bool m_dragging = false;
	glm::vec2 m_dragStartWorld = glm::vec2(0.0f);
	std::vector<DragItem> m_dragItems;
};

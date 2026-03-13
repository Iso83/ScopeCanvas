#include "Interaction/SelectionController.h"

#include <algorithm>

void SelectionController::onMouseDown(DiagramModel &model, const glm::vec2 &mouseWorld, bool shiftDown) {
	Node *hitNode = hitTest(model, mouseWorld);

	if (hitNode != nullptr) {
		if (shiftDown) {
			hitNode->selected = !hitNode->selected;
			return;
		}

		if (!hitNode->selected) {
			for (Node &node : model.nodes()) {
				node.selected = (&node == hitNode);
			}
		}
		return;
	}

	if (!shiftDown) {
		model.clearNodeSelection();
	}

	m_boxSelecting = true;
	m_boxStart = mouseWorld;
	m_boxEnd = mouseWorld;
	updateBoxSelection(model);
}

void SelectionController::onMouseDrag(DiagramModel &model, const glm::vec2 &mouseWorld) {
	if (!m_boxSelecting) {
		return;
	}

	m_boxEnd = mouseWorld;
	updateBoxSelection(model);
}

void SelectionController::onMouseUp(DiagramModel &model, const glm::vec2 &mouseWorld) {
	if (!m_boxSelecting) {
		return;
	}

	m_boxEnd = mouseWorld;
	updateBoxSelection(model);
	m_boxSelecting = false;
}

Node *SelectionController::hitTest(DiagramModel &model, const glm::vec2 &mouseWorld) {
	for (auto it = model.nodes().rbegin(); it != model.nodes().rend(); ++it) {
		Node &node = *it;
		const float minX = node.position.x;
		const float maxX = node.position.x + node.size.x;
		const float minY = node.position.y;
		const float maxY = node.position.y + node.size.y;

		if (mouseWorld.x >= minX && mouseWorld.x <= maxX &&
			mouseWorld.y >= minY && mouseWorld.y <= maxY) {
			return &node;
		}
	}

	return nullptr;
}

bool SelectionController::intersectsSelectionRect(const Node &node,
	const glm::vec2 &boxStart,
	const glm::vec2 &boxEnd) {
	const float rectMinX = std::min(boxStart.x, boxEnd.x);
	const float rectMaxX = std::max(boxStart.x, boxEnd.x);
	const float rectMinY = std::min(boxStart.y, boxEnd.y);
	const float rectMaxY = std::max(boxStart.y, boxEnd.y);

	const float nodeMinX = node.position.x;
	const float nodeMaxX = node.position.x + node.size.x;
	const float nodeMinY = node.position.y;
	const float nodeMaxY = node.position.y + node.size.y;

	const bool overlapX = nodeMaxX >= rectMinX && nodeMinX <= rectMaxX;
	const bool overlapY = nodeMaxY >= rectMinY && nodeMinY <= rectMaxY;
	return overlapX && overlapY;
}

void SelectionController::updateBoxSelection(DiagramModel &model) {
	for (Node &node : model.nodes()) {
		node.selected = intersectsSelectionRect(node, m_boxStart, m_boxEnd);
	}
}

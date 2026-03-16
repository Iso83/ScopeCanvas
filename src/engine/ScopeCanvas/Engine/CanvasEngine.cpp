#include "Engine/CanvasEngine.h"

GraphDocument &CanvasEngine::graph() {
	return m_graph;
}

const GraphDocument &CanvasEngine::graph() const {
	return m_graph;
}

CommandStack &CanvasEngine::commands() {
	return m_commands;
}

const CommandStack &CanvasEngine::commands() const {
	return m_commands;
}

Node *CanvasEngine::createNodeFromType(const std::string &typeId) {
	return m_graph.createNodeOfType(typeId, { 0.0f, 0.0f }, { 200.0f, 120.0f });
}

GraphView *CanvasEngine::createView() {
	m_views.push_back(GraphView{ m_nextViewId++, glm::vec2(0.0f, 0.0f), 1.0f });
	return &m_views.back();
}

GraphView *CanvasEngine::findView(CanvasViewId viewId) {
	for (GraphView &view : m_views) {
		if (view.id == viewId) {
			return &view;
		}
	}

	return nullptr;
}

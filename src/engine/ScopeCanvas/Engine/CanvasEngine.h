#pragma once

#include "Engine/DiagramModel.h"
#include "Interaction/Commands/CommandManager.h"

#include <string>

using GraphDocument = DiagramModel;
using CommandStack = CommandManager;

class CanvasEngine {
public:
	GraphDocument &graph();
	const GraphDocument &graph() const;

	CommandStack &commands();
	const CommandStack &commands() const;

	Node *createNodeFromType(const std::string &typeId);

	GraphView *createView();
	GraphView *findView(CanvasViewId viewId);

private:
	GraphDocument m_graph;
	CommandStack m_commands;
	uint32_t m_nextViewId = 1;
	std::vector<GraphView> m_views;
};

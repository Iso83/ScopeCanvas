#pragma once

#include "Engine/DiagramModel.h"
#include "Interaction/Commands/CommandManager.h"

using GraphDocument = DiagramModel;
using CommandStack = CommandManager;

class CanvasEngine {
public:
    GraphDocument& graph();
    const GraphDocument& graph() const;

    CommandStack& commands();
    const CommandStack& commands() const;

private:
    GraphDocument m_graph;
    CommandStack m_commands;
};

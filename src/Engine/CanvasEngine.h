#pragma once

#include "Engine/DiagramModel.h"
#include "Engine/NodeDefinitionRegistry.h"
#include "Interaction/Commands/CommandManager.h"

#include <string>

using GraphDocument = DiagramModel;
using CommandStack = CommandManager;

class CanvasEngine {
public:
    GraphDocument& graph();
    const GraphDocument& graph() const;

    CommandStack& commands();
    const CommandStack& commands() const;

    NodeDefinitionRegistry& definitions();
    const NodeDefinitionRegistry& definitions() const;

    Node* createNodeFromDefinition(const std::string& type);

private:
    GraphDocument m_graph;
    CommandStack m_commands;
    NodeDefinitionRegistry m_definitions;
};

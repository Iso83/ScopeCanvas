#include "Engine/CanvasEngine.h"

#include <vector>

GraphDocument& CanvasEngine::graph() {
    return m_graph;
}

const GraphDocument& CanvasEngine::graph() const {
    return m_graph;
}

CommandStack& CanvasEngine::commands() {
    return m_commands;
}

const CommandStack& CanvasEngine::commands() const {
    return m_commands;
}

NodeDefinitionRegistry& CanvasEngine::definitions() {
    return m_definitions;
}

const NodeDefinitionRegistry& CanvasEngine::definitions() const {
    return m_definitions;
}

Node* CanvasEngine::createNodeFromDefinition(const std::string& type) {
    const NodeDefinition* definition = m_definitions.find(type);
    if (definition == nullptr) {
        return nullptr;
    }

    std::vector<ConnectorTemplate> connectorTemplates;
    connectorTemplates.reserve(definition->inputs.size() + definition->outputs.size());

    const auto appendConnectors = [&connectorTemplates](const std::vector<ConnectorDefinition>& defs,
                                                        ConnectorSide side) {
        const size_t count = defs.size();
        for (size_t i = 0; i < count; ++i) {
            const float offset = count == 0 ? 0.5f : static_cast<float>(i + 1) / static_cast<float>(count + 1);
            connectorTemplates.push_back(
                ConnectorTemplate{side, offset, defs[i].direction, defs[i].maxConnections});
        }
    };

    appendConnectors(definition->inputs, ConnectorSide::Left);
    appendConnectors(definition->outputs, ConnectorSide::Right);

    return m_graph.createNodeWithConnectors({0.0f, 0.0f},
                                            {200.0f, 120.0f},
                                            connectorTemplates,
                                            definition->type,
                                            definition->title);
}

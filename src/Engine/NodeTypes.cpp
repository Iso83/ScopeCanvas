#include "Engine/NodeTypes.h"

namespace {
std::vector<ConnectorTemplate> makeTemplates(ConnectorSide side,
                                             ConnectorDirection direction,
                                             size_t count) {
    std::vector<ConnectorTemplate> templates;
    templates.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        const float offset = static_cast<float>(i + 1) / static_cast<float>(count + 1);
        templates.push_back(ConnectorTemplate{side, offset, direction});
    }

    return templates;
}
}

void NodeTypeRegistry::registerType(const NodeType& nodeType) {
    if (nodeType.typeId.empty()) {
        return;
    }

    m_types[nodeType.typeId] = nodeType;
}

const NodeType* NodeTypeRegistry::getType(const std::string& typeId) const {
    const auto it = m_types.find(typeId);
    if (it == m_types.end()) {
        return nullptr;
    }

    return &it->second;
}

void NodeTypeRegistry::registerBuiltInTypes() {
    registerType(NodeType{"Number", "Number", {}, makeTemplates(ConnectorSide::Right, ConnectorDirection::Output, 1)});
    registerType(NodeType{"Add", "Add", makeTemplates(ConnectorSide::Left, ConnectorDirection::Input, 2), makeTemplates(ConnectorSide::Right, ConnectorDirection::Output, 1)});
    registerType(NodeType{"Multiply", "Multiply", makeTemplates(ConnectorSide::Left, ConnectorDirection::Input, 2), makeTemplates(ConnectorSide::Right, ConnectorDirection::Output, 1)});
    registerType(NodeType{"Output", "Output", makeTemplates(ConnectorSide::Left, ConnectorDirection::Input, 1), {}});
}

std::vector<ConnectorTemplate> createConnectorTemplatesForType(size_t inputCount, size_t outputCount) {
    std::vector<ConnectorTemplate> templates;
    const std::vector<ConnectorTemplate> inputs = makeTemplates(ConnectorSide::Left, ConnectorDirection::Input, inputCount);
    const std::vector<ConnectorTemplate> outputs = makeTemplates(ConnectorSide::Right, ConnectorDirection::Output, outputCount);
    templates.reserve(inputs.size() + outputs.size());
    templates.insert(templates.end(), inputs.begin(), inputs.end());
    templates.insert(templates.end(), outputs.begin(), outputs.end());
    return templates;
}

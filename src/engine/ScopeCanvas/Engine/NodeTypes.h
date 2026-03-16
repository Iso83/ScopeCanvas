#pragma once

#include "Engine/DiagramModel.h"

#include <string>
#include <unordered_map>
#include <vector>

struct NodeType {
	std::string typeId;
	std::string name;
	bool allowChildren = false;
	bool resizable = true;
	std::vector<ConnectorTemplate> inputs;
	std::vector<ConnectorTemplate> outputs;
};

class NodeTypeRegistry {
public:
	void registerType(const NodeType &nodeType);
	const NodeType *getType(const std::string &typeId) const;

	void registerBuiltInTypes();

private:
	std::unordered_map<std::string, NodeType> m_types;
};

std::vector<ConnectorTemplate> createConnectorTemplatesForType(size_t inputCount, size_t outputCount);

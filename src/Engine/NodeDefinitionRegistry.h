#pragma once

#include "Engine/DiagramModel.h"

#include <string>
#include <unordered_map>
#include <utility>

class NodeDefinitionRegistry {
public:
    const NodeDefinition* find(const std::string& type) const {
        auto it = m_nodes.find(type);
        if (it == m_nodes.end()) {
            return nullptr;
        }

        return &it->second;
    }

    void registerNode(NodeDefinition def) { m_nodes[def.type] = std::move(def); }

private:
    std::unordered_map<std::string, NodeDefinition> m_nodes;
};

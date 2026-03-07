#include "Engine/Persistence/GraphSerializer.h"

#include "Engine/NodeTypes.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <unordered_set>

namespace {
using json = nlohmann::json;

const char* connectorDirectionToString(ConnectorDirection direction) {
    switch (direction) {
    case ConnectorDirection::Input:
        return "Input";
    case ConnectorDirection::Output:
        return "Output";
    }

    return "Input";
}

bool connectorDirectionFromString(const std::string& value, ConnectorDirection& direction) {
    if (value == "Input") {
        direction = ConnectorDirection::Input;
        return true;
    }

    if (value == "Output") {
        direction = ConnectorDirection::Output;
        return true;
    }

    return false;
}
}

bool GraphSerializer::save(const DiagramModel& model, const std::string& filepath) {
    json root;
    root["nodes"] = json::array();
    root["edges"] = json::array();

    for (const Node& node : model.nodes()) {
        json serializedNode;
        serializedNode["id"] = node.id;
        serializedNode["type"] = node.nodeTypeId;
        serializedNode["title"] = node.title;
        serializedNode["position"] = {node.position.x, node.position.y};
        serializedNode["size"] = {node.size.x, node.size.y};
        serializedNode["connectors"] = json::array();

        for (const Connector& connector : node.connectors) {
            serializedNode["connectors"].push_back({
                {"id", connector.id},
                {"offset", connector.offset},
                {"direction", connectorDirectionToString(connector.direction)},
            });
        }

        root["nodes"].push_back(serializedNode);
    }

    for (const Edge& edge : model.edges()) {
        root["edges"].push_back({
            {"id", edge.id},
            {"fromNode", edge.fromNode},
            {"fromConnector", edge.fromConnector},
            {"toNode", edge.toNode},
            {"toConnector", edge.toConnector},
        });
    }

    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << root.dump(4);
    return file.good();
}

bool GraphSerializer::load(DiagramModel& model, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    json root;
    try {
        file >> root;
    } catch (const json::exception&) {
        return false;
    }

    if (!root.is_object() || !root.contains("nodes") || !root["nodes"].is_array() ||
        !root.contains("edges") || !root["edges"].is_array()) {
        return false;
    }

    model.clear();

    std::unordered_set<uint32_t> loadedNodeIds;

    for (const json& serializedNode : root["nodes"]) {
        if (!serializedNode.is_object()) {
            continue;
        }

        if (!serializedNode.contains("id") || !serializedNode["id"].is_number_unsigned() ||
            !serializedNode.contains("type") || !serializedNode["type"].is_string() ||
            !serializedNode.contains("position") || !serializedNode["position"].is_array() ||
            serializedNode["position"].size() != 2 || !serializedNode.contains("size") ||
            !serializedNode["size"].is_array() || serializedNode["size"].size() != 2) {
            continue;
        }

        Node node{};
        node.id = serializedNode["id"].get<uint32_t>();
        node.nodeTypeId = serializedNode["type"].get<std::string>();
        node.title = serializedNode.contains("title") && serializedNode["title"].is_string()
            ? serializedNode["title"].get<std::string>()
            : node.nodeTypeId;
        node.position.x = serializedNode["position"][0].get<float>();
        node.position.y = serializedNode["position"][1].get<float>();
        node.size.x = serializedNode["size"][0].get<float>();
        node.size.y = serializedNode["size"][1].get<float>();
        node.selected = false;

        if (loadedNodeIds.find(node.id) != loadedNodeIds.end()) {
            continue;
        }

        const NodeType* nodeType = model.nodeTypeRegistry().getType(node.nodeTypeId);
        if (nodeType == nullptr) {
            continue;
        }

        std::vector<ConnectorTemplate> connectorTemplates;
        connectorTemplates.reserve(nodeType->inputs.size() + nodeType->outputs.size());
        connectorTemplates.insert(connectorTemplates.end(), nodeType->inputs.begin(), nodeType->inputs.end());
        connectorTemplates.insert(connectorTemplates.end(), nodeType->outputs.begin(), nodeType->outputs.end());

        std::vector<uint32_t> loadedConnectorIds;
        std::vector<ConnectorDirection> loadedDirections;
        if (serializedNode.contains("connectors") && serializedNode["connectors"].is_array()) {
            for (const json& serializedConnector : serializedNode["connectors"]) {
                if (!serializedConnector.is_object()) {
                    continue;
                }

                if (!serializedConnector.contains("id") || !serializedConnector["id"].is_number_unsigned()) {
                    continue;
                }

                loadedConnectorIds.push_back(serializedConnector["id"].get<uint32_t>());

                ConnectorDirection direction = ConnectorDirection::Input;
                if (serializedConnector.contains("direction") && serializedConnector["direction"].is_string()) {
                    (void)connectorDirectionFromString(serializedConnector["direction"].get<std::string>(), direction);
                }

                loadedDirections.push_back(direction);
            }
        }

        node.connectors.clear();
        for (size_t i = 0; i < connectorTemplates.size(); ++i) {
            ConnectorDirection direction = connectorTemplates[i].direction;
            if (i < loadedDirections.size()) {
                direction = loadedDirections[i];
            }

            const uint32_t connectorId = i < loadedConnectorIds.size() ? loadedConnectorIds[i] : 0;
            node.connectors.push_back(Connector{connectorId,
                                                node.id,
                                                connectorTemplates[i].side,
                                                connectorTemplates[i].offset,
                                                direction});
        }

        model.nodes().push_back(node);
        loadedNodeIds.insert(node.id);
    }

    for (const json& serializedEdge : root["edges"]) {
        if (!serializedEdge.is_object()) {
            continue;
        }

        if (!serializedEdge.contains("id") || !serializedEdge["id"].is_number_unsigned() ||
            !serializedEdge.contains("fromNode") || !serializedEdge["fromNode"].is_number_unsigned() ||
            !serializedEdge.contains("fromConnector") ||
            !serializedEdge["fromConnector"].is_number_unsigned() ||
            !serializedEdge.contains("toNode") || !serializedEdge["toNode"].is_number_unsigned() ||
            !serializedEdge.contains("toConnector") || !serializedEdge["toConnector"].is_number_unsigned()) {
            continue;
        }

        Edge edge{};
        edge.id = serializedEdge["id"].get<uint32_t>();
        edge.fromNode = serializedEdge["fromNode"].get<uint32_t>();
        edge.fromConnector = serializedEdge["fromConnector"].get<uint32_t>();
        edge.toNode = serializedEdge["toNode"].get<uint32_t>();
        edge.toConnector = serializedEdge["toConnector"].get<uint32_t>();
        edge.selected = false;

        if (model.findNode(edge.fromNode) == nullptr || model.findNode(edge.toNode) == nullptr) {
            continue;
        }

        if (model.findConnector(edge.fromNode, edge.fromConnector) == nullptr ||
            model.findConnector(edge.toNode, edge.toConnector) == nullptr) {
            continue;
        }

        model.addEdge(edge);
    }

    model.syncIdCounters();
    return true;
}

#include "Engine/Persistence/GraphSerializer.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <unordered_set>

namespace {
using json = nlohmann::json;

const char* connectorSideToString(ConnectorSide side) {
    switch (side) {
    case ConnectorSide::Top:
        return "Top";
    case ConnectorSide::Right:
        return "Right";
    case ConnectorSide::Bottom:
        return "Bottom";
    case ConnectorSide::Left:
        return "Left";
    }

    return "Top";
}

bool connectorSideFromString(const std::string& value, ConnectorSide& side) {
    if (value == "Top") {
        side = ConnectorSide::Top;
        return true;
    }

    if (value == "Right") {
        side = ConnectorSide::Right;
        return true;
    }

    if (value == "Bottom") {
        side = ConnectorSide::Bottom;
        return true;
    }

    if (value == "Left") {
        side = ConnectorSide::Left;
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
        serializedNode["position"] = {node.position.x, node.position.y};
        serializedNode["size"] = {node.size.x, node.size.y};
        serializedNode["connectors"] = json::array();

        for (const Connector& connector : node.connectors) {
            serializedNode["connectors"].push_back({
                {"id", connector.id},
                {"side", connectorSideToString(connector.side)},
                {"offset", connector.offset},
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
            !serializedNode.contains("position") || !serializedNode["position"].is_array() ||
            serializedNode["position"].size() != 2 || !serializedNode.contains("size") ||
            !serializedNode["size"].is_array() || serializedNode["size"].size() != 2 ||
            !serializedNode.contains("connectors") || !serializedNode["connectors"].is_array()) {
            continue;
        }

        Node node{};
        node.id = serializedNode["id"].get<uint32_t>();
        node.position.x = serializedNode["position"][0].get<float>();
        node.position.y = serializedNode["position"][1].get<float>();
        node.size.x = serializedNode["size"][0].get<float>();
        node.size.y = serializedNode["size"][1].get<float>();
        node.selected = false;

        for (const json& serializedConnector : serializedNode["connectors"]) {
            if (!serializedConnector.is_object()) {
                continue;
            }

            if (!serializedConnector.contains("id") || !serializedConnector["id"].is_number_unsigned() ||
                !serializedConnector.contains("side") || !serializedConnector["side"].is_string() ||
                !serializedConnector.contains("offset") || !serializedConnector["offset"].is_number()) {
                continue;
            }

            ConnectorSide side = ConnectorSide::Top;
            const std::string sideText = serializedConnector["side"].get<std::string>();
            if (!connectorSideFromString(sideText, side)) {
                continue;
            }

            node.connectors.push_back(Connector{
                serializedConnector["id"].get<uint32_t>(),
                node.id,
                side,
                serializedConnector["offset"].get<float>(),
            });
        }

        if (loadedNodeIds.find(node.id) != loadedNodeIds.end()) {
            continue;
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

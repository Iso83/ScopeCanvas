#include "Interaction/Commands/GraphCommands.h"

#include <utility>

CreateNodeCommand::CreateNodeCommand(DiagramModel& model,
                                     const std::string& nodeTypeId,
                                     const glm::vec2& position,
                                     const glm::vec2& size)
    : m_model(model) {
    m_node.nodeTypeId = nodeTypeId;
    m_node.position = position;
    m_node.size = size;
}

void CreateNodeCommand::execute() {
    if (!m_hasNodeData) {
        Node* node = m_model.createNodeOfType(m_node.nodeTypeId, m_node.position, m_node.size);
        if (node == nullptr) {
            return;
        }

        m_node = *node;
        m_hasNodeData = true;
        return;
    }

    (void)m_model.addNode(m_node);
}

void CreateNodeCommand::undo() {
    if (!m_hasNodeData) {
        return;
    }

    (void)m_model.removeNode(m_node.id);
}

DeleteNodeCommand::DeleteNodeCommand(DiagramModel& model, uint32_t nodeId)
    : m_model(model), m_nodeId(nodeId) {}

void DeleteNodeCommand::execute() {
    const Node* node = m_model.findNode(m_nodeId);
    if (node == nullptr) {
        m_valid = false;
        return;
    }

    m_deletedNode = *node;
    m_deletedEdges.clear();
    for (const Edge& edge : m_model.edges()) {
        if (edge.fromNode == m_nodeId || edge.toNode == m_nodeId) {
            m_deletedEdges.push_back(edge);
        }
    }

    m_valid = m_model.removeNode(m_nodeId);
}

void DeleteNodeCommand::undo() {
    if (!m_valid) {
        return;
    }

    if (m_model.findNode(m_deletedNode.id) == nullptr) {
        (void)m_model.addNode(m_deletedNode);
    }

    for (const Edge& edge : m_deletedEdges) {
        if (m_model.findEdge(edge.id) == nullptr) {
            (void)m_model.addEdge(edge);
        }
    }
}

MoveNodesCommand::MoveNodesCommand(DiagramModel& model, std::vector<MoveItem> moveItems)
    : m_model(model), m_moveItems(std::move(moveItems)) {}

void MoveNodesCommand::execute() {
    for (const MoveItem& item : m_moveItems) {
        Node* node = m_model.findNode(item.nodeId);
        if (node == nullptr) {
            continue;
        }

        node->position = item.endPosition;
    }
}

void MoveNodesCommand::undo() {
    for (const MoveItem& item : m_moveItems) {
        Node* node = m_model.findNode(item.nodeId);
        if (node == nullptr) {
            continue;
        }

        node->position = item.startPosition;
    }
}

CreateEdgeCommand::CreateEdgeCommand(DiagramModel& model, const Edge& edge) : m_model(model), m_edge(edge) {}

void CreateEdgeCommand::execute() {
    m_created = m_model.addEdge(m_edge);
}

void CreateEdgeCommand::undo() {
    if (!m_created) {
        return;
    }

    (void)m_model.removeEdge(m_edge.id);
}

DeleteEdgeCommand::DeleteEdgeCommand(DiagramModel& model, uint32_t edgeId) : m_model(model) {
    const Edge* edge = m_model.findEdge(edgeId);
    if (edge != nullptr) {
        m_deletedEdge = *edge;
        m_valid = true;
    }
}

void DeleteEdgeCommand::execute() {
    if (!m_valid) {
        return;
    }

    (void)m_model.removeEdge(m_deletedEdge.id);
}

void DeleteEdgeCommand::undo() {
    if (!m_valid) {
        return;
    }

    (void)m_model.addEdge(m_deletedEdge);
}

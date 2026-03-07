#pragma once

#include "Engine/DiagramModel.h"
#include "Interaction/Commands/Command.h"

#include <glm/vec2.hpp>

#include <vector>

class CreateNodeCommand : public Command {
public:
    CreateNodeCommand(DiagramModel& model, const glm::vec2& position, const glm::vec2& size);

    void execute() override;
    void undo() override;

    uint32_t nodeId() const { return m_node.id; }

private:
    DiagramModel& m_model;
    Node m_node{};
    bool m_hasNodeData = false;
};

class DeleteNodeCommand : public Command {
public:
    DeleteNodeCommand(DiagramModel& model, uint32_t nodeId);

    void execute() override;
    void undo() override;

private:
    DiagramModel& m_model;
    uint32_t m_nodeId = 0;
    Node m_deletedNode{};
    std::vector<Edge> m_deletedEdges;
    bool m_valid = false;
};

class MoveNodesCommand : public Command {
public:
    struct MoveItem {
        uint32_t nodeId = 0;
        glm::vec2 startPosition = glm::vec2(0.0f);
        glm::vec2 endPosition = glm::vec2(0.0f);
    };

    MoveNodesCommand(DiagramModel& model, std::vector<MoveItem> moveItems);

    bool empty() const { return m_moveItems.empty(); }

    void execute() override;
    void undo() override;

private:
    DiagramModel& m_model;
    std::vector<MoveItem> m_moveItems;
};

class CreateEdgeCommand : public Command {
public:
    CreateEdgeCommand(DiagramModel& model, const Edge& edge);

    void execute() override;
    void undo() override;

    uint32_t edgeId() const { return m_edge.id; }

private:
    DiagramModel& m_model;
    Edge m_edge{};
    bool m_created = false;
};

class DeleteEdgeCommand : public Command {
public:
    DeleteEdgeCommand(DiagramModel& model, uint32_t edgeId);

    void execute() override;
    void undo() override;

private:
    DiagramModel& m_model;
    Edge m_deletedEdge{};
    bool m_valid = false;
};

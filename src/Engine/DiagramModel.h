#pragma once

#include <glm/vec2.hpp>

#include <vector>

struct Node {
    int id;
    glm::vec2 position;
    glm::vec2 size;
    bool selected;
};

class DiagramModel {
public:
    DiagramModel() = default;

    std::vector<Node>& nodes() { return m_nodes; }
    const std::vector<Node>& nodes() const { return m_nodes; }

private:
    std::vector<Node> m_nodes;
};

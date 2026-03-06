#pragma once

#include <string>
#include <vector>

namespace Engine {

struct Vec2 {
    float x {0.0F};
    float y {0.0F};
};

struct Node {
    std::string id;
    Vec2 position;
    Vec2 size {160.0F, 100.0F};
    bool selected {false};
};

class DiagramModel {
public:
    DiagramModel();

    const std::vector<Node>& nodes() const noexcept;
    std::vector<Node>& nodes() noexcept;

private:
    std::vector<Node> m_nodes;
};

} // namespace Engine

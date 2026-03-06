#include "DiagramModel.h"

namespace Engine {

DiagramModel::DiagramModel() {
    m_nodes.push_back(Node{"Node A", Vec2{0.0F, 0.0F}, Vec2{160.0F, 100.0F}, false});
    m_nodes.push_back(Node{"Node B", Vec2{200.0F, 100.0F}, Vec2{180.0F, 100.0F}, true});
    m_nodes.push_back(Node{"Node C", Vec2{-150.0F, -50.0F}, Vec2{150.0F, 100.0F}, false});
}

const std::vector<Node>& DiagramModel::nodes() const noexcept {
    return m_nodes;
}

std::vector<Node>& DiagramModel::nodes() noexcept {
    return m_nodes;
}

} // namespace Engine

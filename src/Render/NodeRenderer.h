#pragma once

#include "Engine/DiagramModel.h"
#include "View/Camera2D.h"

#include <vector>

namespace Render {

struct NodeVertex {
    float x;
    float y;
    float r;
    float g;
    float b;
};

class NodeRenderer {
public:
    void draw(const Engine::DiagramModel& model, const View::Camera2D& camera);

private:
    void rebuildVertexBuffer(const Engine::DiagramModel& model, const View::Camera2D& camera);

    std::vector<NodeVertex> m_vbo;
};

} // namespace Render

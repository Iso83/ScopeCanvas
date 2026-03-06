#pragma once

#include "Engine/DiagramModel.h"
#include "Render/EdgeRenderer.h"
#include "Render/GridRenderer.h"
#include "Render/NodeRenderer.h"
#include "View/Camera2D.h"

namespace Render {

class Renderer {
public:
    void render(const Engine::DiagramModel& model, const View::Camera2D& camera);

private:
    GridRenderer m_gridRenderer;
    EdgeRenderer m_edgeRenderer;
    NodeRenderer m_nodeRenderer;
};

} // namespace Render

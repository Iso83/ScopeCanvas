#pragma once

#include "Engine/DiagramModel.h"
#include "Render/GridRenderer.h"
#include "Render/NodeRenderer.h"
#include "View/Camera2D.h"

class Renderer {
public:
    Renderer();

    bool init(const char* gridVertexShaderPath,
              const char* gridFragmentShaderPath,
              const char* nodeVertexShaderPath,
              const char* nodeFragmentShaderPath,
              int viewportWidth,
              int viewportHeight);
    void resize(int width, int height);
    void render(const DiagramModel& model);

    Camera2D& camera() { return m_camera; }

private:
    Camera2D m_camera;
    GridRenderer m_gridRenderer;
    NodeRenderer m_nodeRenderer;
    int m_viewportWidth;
    int m_viewportHeight;
};

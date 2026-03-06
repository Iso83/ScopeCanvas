#pragma once

#include "Render/GridRenderer.h"
#include "View/Camera2D.h"

class Renderer {
public:
    Renderer();

    bool init(const char* gridVertexShaderPath, const char* gridFragmentShaderPath, int viewportWidth, int viewportHeight);
    void resize(int width, int height);
    void render();

    Camera2D& camera() { return m_camera; }

private:
    Camera2D m_camera;
    GridRenderer m_gridRenderer;
    int m_viewportWidth;
    int m_viewportHeight;
};

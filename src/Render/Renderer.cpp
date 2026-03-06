#include "Render/Renderer.h"

#include <glad/glad.h>

Renderer::Renderer() : m_viewportWidth(1280), m_viewportHeight(720) {}

bool Renderer::init(const char* gridVertexShaderPath, const char* gridFragmentShaderPath, int viewportWidth, int viewportHeight) {
    m_viewportWidth = viewportWidth;
    m_viewportHeight = viewportHeight;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_camera.setViewportSize(viewportWidth, viewportHeight);
    return m_gridRenderer.init(gridVertexShaderPath, gridFragmentShaderPath);
}

void Renderer::resize(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
    m_camera.setViewportSize(width, height);
    glViewport(0, 0, width, height);
}

void Renderer::render() {
    glClearColor(0.10f, 0.11f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_gridRenderer.render(m_camera.viewProjection(), m_viewportWidth, m_viewportHeight);
}

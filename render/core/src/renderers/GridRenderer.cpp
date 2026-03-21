#include "renderers/GridRenderer.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <limits>
#include <vector>

namespace ScopeCanvas::Render::Renderers {
GridRenderer::GridRenderer() : m_vao(0), m_vbo(0) {}

GridRenderer::~GridRenderer() {
    destroy();
}

bool GridRenderer::init() {
    if (!m_shader.load(GL::GridVertex, GL::GridFragment)) {
        return false;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(sizeof(glm::vec2)), nullptr);
    glBindVertexArray(0);

    return true;
}

void GridRenderer::render(const glm::mat4& viewProjection, int viewportWidth, int viewportHeight, float cellSize) {
    (void)viewportWidth;
    (void)viewportHeight;

    const float spacing = std::max(cellSize, 1.0F);
    const glm::mat4 invViewProjection = glm::inverse(viewProjection);

    const glm::vec4 cornersNdc[] = {
        {-1.0F, -1.0F, 0.0F, 1.0F}, {1.0F, -1.0F, 0.0F, 1.0F}, {-1.0F, 1.0F, 0.0F, 1.0F}, {1.0F, 1.0F, 0.0F, 1.0F}};

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();

    for (const glm::vec4& cornerNdc : cornersNdc) {
        const glm::vec4 world = invViewProjection * cornerNdc;
        const glm::vec2 worldPos = glm::vec2(world) / world.w;
        minX = std::min(minX, worldPos.x);
        maxX = std::max(maxX, worldPos.x);
        minY = std::min(minY, worldPos.y);
        maxY = std::max(maxY, worldPos.y);
    }

    const float paddedMinX = minX - spacing;
    const float paddedMaxX = maxX + spacing;
    const float paddedMinY = minY - spacing;
    const float paddedMaxY = maxY + spacing;

    const float startX = std::floor(paddedMinX / spacing) * spacing;
    const float endX = std::ceil(paddedMaxX / spacing) * spacing;
    const float startY = std::floor(paddedMinY / spacing) * spacing;
    const float endY = std::ceil(paddedMaxY / spacing) * spacing;

    std::vector<glm::vec2> minorLines;
    std::vector<glm::vec2> majorLines;

    const float epsilon = spacing * 0.1F;

    for (float x = startX; x <= endX + epsilon; x += spacing) {
        const int index = static_cast<int>(std::llround(x / spacing));
        const bool major = (index % 5 == 0);
        std::vector<glm::vec2>& lines = major ? majorLines : minorLines;
        lines.push_back({x, paddedMinY});
        lines.push_back({x, paddedMaxY});
    }

    for (float y = startY; y <= endY + epsilon; y += spacing) {
        const int index = static_cast<int>(std::llround(y / spacing));
        const bool major = (index % 5 == 0);
        std::vector<glm::vec2>& lines = major ? majorLines : minorLines;
        lines.push_back({paddedMinX, y});
        lines.push_back({paddedMaxX, y});
    }

    uploadAndDrawLines(minorLines, viewProjection, {0.20F, 0.22F, 0.24F});
    uploadAndDrawLines(majorLines, viewProjection, {0.32F, 0.35F, 0.38F});
}

void GridRenderer::uploadAndDrawLines(const std::vector<glm::vec2>& vertices, const glm::mat4& viewProjection,
                                      const glm::vec3& color) {
    if (vertices.empty()) {
        return;
    }

    m_shader.use();

    const GLuint programId = m_shader.id();
    const GLint vpLoc = glGetUniformLocation(programId, "uViewProjection");
    const GLint colorLoc = glGetUniformLocation(programId, "uColor");

    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform3f(colorLoc, color.x, color.y, color.z);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec2)), vertices.data(),
                 GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}

void GridRenderer::destroy() {
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}
} // namespace ScopeCanvas::Render::Renderers

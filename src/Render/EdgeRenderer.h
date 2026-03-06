#pragma once

#include "Engine/DiagramModel.h"
#include "Render/Shader.h"

#include <glad/glad.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstddef>
#include <vector>

class EdgeRenderer {
public:
    EdgeRenderer();
    ~EdgeRenderer();

    EdgeRenderer(const EdgeRenderer&) = delete;
    EdgeRenderer& operator=(const EdgeRenderer&) = delete;

    bool init(const char* vertexShaderPath, const char* fragmentShaderPath);

    void renderEdges(const DiagramModel& model, const glm::mat4& viewProjection);
    void renderConnectors(const std::vector<Node>& nodes, const glm::mat4& viewProjection);
    void renderPreviewEdge(const DiagramModel& model,
                           uint32_t startNodeId,
                           uint32_t startConnectorId,
                           const glm::vec2& previewPosition,
                           const glm::mat4& viewProjection);

private:
    static void appendBezierSamples(std::vector<glm::vec2>& points,
                                    const glm::vec2& p0,
                                    const glm::vec2& p1,
                                    const glm::vec2& p2,
                                    const glm::vec2& p3,
                                    int segments);

    static void computeBezierControls(const glm::vec2& p0,
                                      const glm::vec2& p3,
                                      glm::vec2& p1,
                                      glm::vec2& p2);

    void ensureBufferCapacity(size_t pointCount);

    void renderPolyline(const std::vector<glm::vec2>& points,
                        const glm::mat4& viewProjection,
                        const glm::vec3& color,
                        float thickness);

    Shader m_shader;
    GLuint m_vao;
    GLuint m_vbo;
    mutable std::vector<glm::vec2> m_scratchPoints;
    size_t m_vboCapacityBytes;
};

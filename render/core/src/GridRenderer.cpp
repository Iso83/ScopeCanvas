#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/render/GridRenderer.h>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

namespace ScopeCanvas::Render {
namespace {
unsigned int compile(unsigned int type, const char* src) {
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

float zoomAdjustedSpacing(float baseCellSize, float zoom) {
    float spacing = std::max(baseCellSize, 1.0F);
    const float visiblePixels = spacing * zoom;

    if (visiblePixels < 18.0F) {
        while (spacing * zoom < 18.0F) {
            spacing *= 2.0F;
        }
    } else if (visiblePixels > 96.0F) {
        while (spacing * zoom > 96.0F && spacing > 1.0F) {
            spacing *= 0.5F;
        }
    }

    return std::max(spacing, 1.0F);
}
} // namespace

GridRenderer::GridRenderer() : m_vao(0), m_vbo(0) {}

GridRenderer::~GridRenderer() {
    destroy();
}

bool GridRenderer::init() {
    const char* vs = R"(#version 330 core
layout(location = 0) in vec2 aPos;
out vec2 vUv;
void main() {
    vUv = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
})";

    const char* fs = R"(#version 330 core
in vec2 vUv;
out vec4 FragColor;

uniform mat4 uInvViewProjection;
uniform vec2 uViewportSize;
uniform float uMinorSpacing;
uniform float uMajorSpacing;

float gridLineAlpha(vec2 worldPos, float spacing) {
    vec2 grid = abs(fract(worldPos / spacing - 0.5) - 0.5) / fwidth(worldPos / spacing);
    float line = min(grid.x, grid.y);
    return 1.0 - min(line, 1.0);
}

void main() {
    vec2 frag = gl_FragCoord.xy / uViewportSize;
    vec2 ndc = vec2(frag.x * 2.0 - 1.0, frag.y * 2.0 - 1.0);
    vec4 world = uInvViewProjection * vec4(ndc, 0.0, 1.0);
    vec2 worldPos = world.xy / world.w;

    float minor = gridLineAlpha(worldPos, uMinorSpacing);
    float major = gridLineAlpha(worldPos, uMajorSpacing);

    vec3 color = vec3(0.08, 0.09, 0.11);
    color += vec3(0.10, 0.11, 0.13) * minor;
    color += vec3(0.14, 0.16, 0.19) * major;

    float alpha = max(minor * 0.55, major * 0.8);
    FragColor = vec4(color, alpha);
})";

    const unsigned int vertexShader = compile(GL_VERTEX_SHADER, vs);
    const unsigned int fragmentShader = compile(GL_FRAGMENT_SHADER, fs);

    const unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    m_shader = GL::Shader{};
    if (program == 0)
        return false;

    // Borrow the lightweight wrapper for use()/id().
    // Re-load with equivalent sources through the wrapper so ownership stays consistent.
    if (!m_shader.load(vs, fs)) {
        glDeleteProgram(program);
        return false;
    }
    glDeleteProgram(program);

    constexpr float quad[] = {
        -1.0F, -1.0F, 1.0F, -1.0F, 1.0F, 1.0F, -1.0F, -1.0F, 1.0F, 1.0F, -1.0F, 1.0F,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glBindVertexArray(0);

    return true;
}

void GridRenderer::shutdown() {
    destroy();
}

void GridRenderer::render(const Camera::Camera2D& camera, float cellSize) const {
    const float minorSpacing = zoomAdjustedSpacing(cellSize, camera.zoom());
    const float majorSpacing = minorSpacing * 5.0F;

    m_shader.use();
    const GLuint programId = m_shader.id();
    const GLint invVpLoc = glGetUniformLocation(programId, "uInvViewProjection");
    const GLint viewportLoc = glGetUniformLocation(programId, "uViewportSize");
    const GLint minorLoc = glGetUniformLocation(programId, "uMinorSpacing");
    const GLint majorLoc = glGetUniformLocation(programId, "uMajorSpacing");

    const glm::mat4 invViewProjection = camera.invViewProjection();
    glUniformMatrix4fv(invVpLoc, 1, GL_FALSE, glm::value_ptr(invViewProjection));

    GLint viewport[4]{};
    glGetIntegerv(GL_VIEWPORT, viewport);
    glUniform2f(viewportLoc, static_cast<float>(std::max(viewport[2], 1)),
                static_cast<float>(std::max(viewport[3], 1)));
    glUniform1f(minorLoc, minorSpacing);
    glUniform1f(majorLoc, majorSpacing);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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
} // namespace ScopeCanvas::Render

#include <ScopeCanvas/engine/render/camera/Camera2D.h>
#include <ScopeCanvas/engine/render/geometry/RoundedRect.h>
#include <ScopeCanvas/engine/render/gl/OpenGLApi.h>
#include <ScopeCanvas/engine/render/gl/ShaderSource.h>
#include <ScopeCanvas/widget/render/NodeInfo.h>
#include <algorithm>
#include <cmath>
#include <utility>

using namespace ScopeCanvas::Engine::Render;
using namespace ScopeCanvas::Engine::Render::Text;

namespace ScopeCanvas::Widget::Render {
namespace {
struct Vertex {
    float x;
    float y;
    float r;
    float g;
    float b;
    float a;
};

unsigned int compile(unsigned int type, const char* source) {
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

std::string defaultFontPath() {
#if defined(__EMSCRIPTEN__)
    return "/assets/DejaVuSans.ttf";
#elif defined(_WIN32)
    return "C:/Windows/Fonts/segoeui.ttf";
#elif defined(__APPLE__)
    return "/System/Library/Fonts/Supplemental/Arial.ttf";
#else
    return "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
#endif
}

glm::vec4 mixColor(const glm::vec4& a, const glm::vec4& b, float t) {
    return a + (b - a) * t;
}

glm::vec4 brighten(const glm::vec4& color, float amount) {
    return mixColor(color, glm::vec4{1.0F}, amount);
}

void appendVertex(std::vector<Vertex>& vertices, glm::vec2 point, glm::vec4 color) {
    vertices.push_back({point.x, point.y, color.r, color.g, color.b, color.a});
}

void appendTriangle(std::vector<Vertex>& vertices, glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec4 color) {
    appendVertex(vertices, a, color);
    appendVertex(vertices, b, color);
    appendVertex(vertices, c, color);
}

glm::vec4 horizontalGradient(glm::vec2 point, glm::vec2 minPoint, glm::vec2 size, glm::vec4 left, glm::vec4 right) {
    const float t = std::clamp((point.x - minPoint.x) / std::max(size.x, 1.0F), 0.0F, 1.0F);
    return mixColor(left, right, t);
}

void appendRoundedRect(std::vector<Vertex>& vertices, glm::vec2 position, glm::vec2 size, float radius,
                       glm::vec4 color) {
    const std::vector<glm::vec2> points = Geometry::roundedRectOutline(position, size, radius, 8);
    if (points.size() < 3U)
        return;

    glm::vec2 center{0.0F};
    for (glm::vec2 point : points)
        center += point;
    center /= static_cast<float>(points.size());

    for (std::size_t i = 0; i < points.size(); ++i)
        appendTriangle(vertices, center, points[i], points[(i + 1U) % points.size()], color);
}

void appendRoundedGradientRect(std::vector<Vertex>& vertices, glm::vec2 position, glm::vec2 size, float radius,
                               glm::vec4 left, glm::vec4 right) {
    const std::vector<glm::vec2> points = Geometry::roundedRectOutline(position, size, radius, 8);
    if (points.size() < 3U)
        return;

    glm::vec2 center{0.0F};
    for (glm::vec2 point : points)
        center += point;
    center /= static_cast<float>(points.size());

    for (std::size_t i = 0; i < points.size(); ++i) {
        const glm::vec2 a = points[i];
        const glm::vec2 b = points[(i + 1U) % points.size()];
        appendVertex(vertices, center, horizontalGradient(center, position, size, left, right));
        appendVertex(vertices, a, horizontalGradient(a, position, size, left, right));
        appendVertex(vertices, b, horizontalGradient(b, position, size, left, right));
    }
}

std::vector<glm::vec2> topRoundedRectOutline(glm::vec2 position, glm::vec2 size, float radius, int segments) {
    const float clampedRadius = std::clamp(radius, 0.0F, std::min(size.x, size.y) * 0.5F);
    const glm::vec2 min = position;
    const glm::vec2 max = position + size;

    if (clampedRadius <= 0.01F) {
        return {{min.x, min.y}, {max.x, min.y}, {max.x, max.y}, {min.x, max.y}};
    }

    std::vector<glm::vec2> points;
    points.reserve(static_cast<std::size_t>(segments) * 2U + 6U);
    points.push_back({min.x, min.y});
    points.push_back({max.x, min.y});

    const glm::vec2 topRightCenter{max.x - clampedRadius, max.y - clampedRadius};
    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const float angle = t * 1.57079632679F;
        points.push_back(topRightCenter + glm::vec2{std::cos(angle), std::sin(angle)} * clampedRadius);
    }

    const glm::vec2 topLeftCenter{min.x + clampedRadius, max.y - clampedRadius};
    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const float angle = 1.57079632679F + t * 1.57079632679F;
        points.push_back(topLeftCenter + glm::vec2{std::cos(angle), std::sin(angle)} * clampedRadius);
    }

    return points;
}

void appendTopRoundedGradientRect(std::vector<Vertex>& vertices, glm::vec2 position, glm::vec2 size, float radius,
                                  glm::vec4 left, glm::vec4 right) {
    const std::vector<glm::vec2> points = topRoundedRectOutline(position, size, radius, 8);
    if (points.size() < 3U)
        return;

    glm::vec2 center{0.0F};
    for (glm::vec2 point : points)
        center += point;
    center /= static_cast<float>(points.size());

    for (std::size_t i = 0; i < points.size(); ++i) {
        const glm::vec2 a = points[i];
        const glm::vec2 b = points[(i + 1U) % points.size()];
        appendVertex(vertices, center, horizontalGradient(center, position, size, left, right));
        appendVertex(vertices, a, horizontalGradient(a, position, size, left, right));
        appendVertex(vertices, b, horizontalGradient(b, position, size, left, right));
    }
}

void appendCircle(std::vector<Vertex>& vertices, glm::vec2 center, float radius, glm::vec4 color) {
    constexpr int kSegments = 18;
    for (int i = 0; i < kSegments; ++i) {
        const float a0 = static_cast<float>(i) / static_cast<float>(kSegments) * 6.28318530718F;
        const float a1 = static_cast<float>(i + 1) / static_cast<float>(kSegments) * 6.28318530718F;
        appendTriangle(vertices, center, center + glm::vec2{std::cos(a0), std::sin(a0)} * radius,
                       center + glm::vec2{std::cos(a1), std::sin(a1)} * radius, color);
    }
}

void appendIconShape(std::vector<Vertex>& vertices, Render::Theme::NodeIconShape shape, glm::vec2 position,
                     glm::vec2 size, glm::vec4 color) {
    const glm::vec2 center = position + size * 0.5F;
    const float unit = std::min(size.x, size.y);

    switch (shape) {
        case Render::Theme::NodeIconShape::Square:
            appendRoundedRect(vertices, position + size * 0.23F, size * 0.54F, unit * 0.08F, color);
            break;
        case Render::Theme::NodeIconShape::Circle:
            appendCircle(vertices, center, unit * 0.28F, color);
            break;
        case Render::Theme::NodeIconShape::Triangle:
            appendTriangle(vertices, {center.x, position.y + size.y * 0.76F},
                           {position.x + size.x * 0.26F, position.y + size.y * 0.24F},
                           {position.x + size.x * 0.74F, position.y + size.y * 0.24F}, color);
            break;
        case Render::Theme::NodeIconShape::Diamond:
            appendTriangle(vertices, {position.x + size.x * 0.24F, center.y}, {center.x, position.y + size.y * 0.76F},
                           {position.x + size.x * 0.76F, center.y}, color);
            appendTriangle(vertices, {position.x + size.x * 0.24F, center.y}, {center.x, position.y + size.y * 0.24F},
                           {position.x + size.x * 0.76F, center.y}, color);
            break;
        case Render::Theme::NodeIconShape::Chevrons:
            appendTriangle(vertices, {position.x + size.x * 0.28F, position.y + size.y * 0.22F},
                           {position.x + size.x * 0.62F, center.y},
                           {position.x + size.x * 0.28F, position.y + size.y * 0.78F}, color);
            appendTriangle(vertices, {position.x + size.x * 0.48F, position.y + size.y * 0.22F},
                           {position.x + size.x * 0.82F, center.y},
                           {position.x + size.x * 0.48F, position.y + size.y * 0.78F}, color);
            break;
        default:
            appendCircle(vertices, center, unit * 0.24F, color);
            break;
    }
}

} // namespace

NodeInfoRenderer::NodeInfoRenderer(NodeInfoFontOptions fontOptions) : m_fontOptions(std::move(fontOptions)) {
    if (m_fontOptions.fontPath.empty())
        m_fontOptions.fontPath = defaultFontPath();
}

bool NodeInfoRenderer::init() {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(2 * sizeof(float)));
    glBindVertexArray(0);

    const std::string geometryVs = std::string(GL::ShaderVersionPrefix) + R"(

        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec4 aColor;

        uniform mat4 uVP;

        out vec4 vColor;

        void main(){
            gl_Position = uVP * vec4(aPos, 0.0, 1.0); vColor = aColor;
        }
    )";
    const std::string geometryFs = std::string(GL::ShaderVersionPrefix) + R"(

        in vec4 vColor;

        out vec4 FragColor;

        void main(){
            FragColor = vColor;
        }
    )";
    const unsigned int gv = compile(GL_VERTEX_SHADER, geometryVs.c_str());
    const unsigned int gf = compile(GL_FRAGMENT_SHADER, geometryFs.c_str());
    m_program = glCreateProgram();
    glAttachShader(m_program, gv);
    glAttachShader(m_program, gf);
    glLinkProgram(m_program);
    glDeleteShader(gv);
    glDeleteShader(gf);

    const float atlasScale = std::max(m_fontOptions.atlasScale, 1.0F);
    const auto pixelSize =
        static_cast<unsigned int>(std::ceil(std::max(m_fontOptions.titleSize, m_fontOptions.iconSize) * atlasScale));
    if (!m_textRenderer.init() || !m_textRenderer.loadFont(m_fontOptions.fontPath, pixelSize)) {
        shutdown();
        return false;
    }
    return true;
}

void NodeInfoRenderer::shutdown() {
    m_textRenderer.shutdown();
    if (m_program != 0)
        glDeleteProgram(m_program);
    if (m_vbo != 0)
        glDeleteBuffers(1, &m_vbo);
    if (m_vao != 0)
        glDeleteVertexArrays(1, &m_vao);
    m_program = m_vbo = m_vao = 0;
}

void NodeInfoRenderer::render(const std::vector<Scene::NodeRenderData>& nodes, const Camera::Camera2D& camera,
                              const Theme::NodeVisualRegistry& registry) const {
    struct TextRun {
        std::string value;
        glm::vec2 position;
        glm::vec2 size;
        float fontSize;
        glm::vec4 color;
    };
    auto appendText = [](std::vector<TextRun>& runs, const std::string& value, glm::vec2 position, glm::vec2 size,
                         float fontSize, glm::vec4 color) { runs.push_back({value, position, size, fontSize, color}); };

    std::vector<Vertex> geometry;
    std::vector<TextRun> text;
    geometry.reserve(nodes.size() * 128U);

    for (const Scene::NodeRenderData& node : nodes) {
        const Theme::NodeVisual& visual = registry.getVisual(node.typeId);
        const float borderThickness = std::clamp(visual.style.borderThickness, 1.0F, 3.5F);
        const glm::vec2 innerPosition = node.position + glm::vec2(borderThickness);
        const glm::vec2 innerSize = node.size - glm::vec2(borderThickness * 2.0F);
        const float headerHeight = std::clamp(visual.headerHeight, 18.0F, std::max(18.0F, innerSize.y - 4.0F));
        const glm::vec2 headerPosition{innerPosition.x, innerPosition.y + innerSize.y - headerHeight};
        const glm::vec2 headerSize{innerSize.x, headerHeight};
        const float radius =
            std::clamp(visual.style.cornerRadius - borderThickness, 2.0F, std::min(node.size.x, node.size.y) * 0.22F);

        const glm::vec4 headerLeft = mixColor(visual.headerColor, visual.style.bodyColor, 0.28F);
        const glm::vec4 headerRight = mixColor(visual.headerAccentColor, visual.style.bodyColor, 0.38F);
        appendTopRoundedGradientRect(geometry, headerPosition, headerSize, radius, headerLeft, headerRight);

        const float iconBoxExtent = std::min(24.0F, std::max(14.0F, headerHeight - 12.0F));
        const glm::vec2 iconBoxSize{iconBoxExtent, iconBoxExtent};
        const glm::vec2 iconBoxPosition = headerPosition + glm::vec2{7.0F, (headerHeight - iconBoxExtent) * 0.5F};
        appendRoundedGradientRect(geometry, iconBoxPosition, iconBoxSize, 4.0F, brighten(headerLeft, 0.14F),
                                  brighten(headerRight, 0.14F));
        appendIconShape(geometry, visual.iconShape, iconBoxPosition, iconBoxSize, visual.iconColor);

        const float textLeft = iconBoxPosition.x + iconBoxSize.x + 8.0F;
        const glm::vec2 textPosition{textLeft, headerPosition.y + 7.0F};
        const glm::vec2 textSize{std::max(1.0F, headerPosition.x + headerSize.x - textLeft - 22.0F),
                                 std::max(1.0F, headerHeight - 12.0F)};
        if (visual.subtitle.empty()) {
            appendText(text, visual.title, textPosition, textSize, m_fontOptions.titleSize, visual.textColor);
        } else {
            const float titleLineHeight = std::min(22.0F, textSize.y * 0.58F);
            appendText(text, visual.title, textPosition + glm::vec2{0.0F, textSize.y - titleLineHeight},
                       {textSize.x, titleLineHeight}, m_fontOptions.titleSize, visual.textColor);
            appendText(text, visual.subtitle, textPosition, {textSize.x, textSize.y - titleLineHeight + 2.0F},
                       m_fontOptions.iconSize, {0.72F, 0.82F, 0.92F, 1.0F});
        }
    }

    const glm::mat4 vp = camera.viewProjection();
    if (!geometry.empty()) {
        glUseProgram(m_program);
        glUniformMatrix4fv(glGetUniformLocation(m_program, "uVP"), 1, GL_FALSE, &vp[0][0]);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(geometry.size() * sizeof(Vertex)), geometry.data(),
                     GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(geometry.size()));
    }

    const FontMetrics metrics = m_textRenderer.fontMetrics();
    for (const TextRun& run : text) {
        const float scale = run.fontSize / static_cast<float>(metrics.pixelSize);
        const float baselineY =
            run.position.y + (run.size.y - (metrics.ascent - metrics.descent) * scale) * 0.5F - metrics.descent * scale;
        m_textRenderer.render(run.value, {run.position.x, baselineY}, run.color, vp, ClipRect{run.position, run.size},
                              scale);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void NodeInfoRenderer::render(const std::vector<NodeInfo>& nodes, const Camera::Camera2D& camera) const {
    struct TextRun {
        std::string value;
        glm::vec2 position;
        glm::vec2 size;
        float fontSize;
        glm::vec4 color;
    };
    auto appendText = [](std::vector<TextRun>& runs, const std::string& value, glm::vec2 position, glm::vec2 size,
                         float fontSize, glm::vec4 color) { runs.push_back({value, position, size, fontSize, color}); };

    std::vector<Vertex> geometry;
    std::vector<TextRun> text;
    geometry.reserve(nodes.size() * 128U);

    for (const NodeInfo& node : nodes) {
        const Render::Theme::NodeVisual& visual = node.visual;
        const float borderThickness = std::clamp(visual.style.borderThickness, 1.0F, 3.5F);
        const glm::vec2 innerPosition = node.position + glm::vec2(borderThickness);
        const glm::vec2 innerSize = node.size - glm::vec2(borderThickness * 2.0F);
        const float headerHeight = std::clamp(visual.headerHeight, 18.0F, std::max(18.0F, innerSize.y - 4.0F));
        const glm::vec2 headerPosition{innerPosition.x, innerPosition.y + innerSize.y - headerHeight};
        const glm::vec2 headerSize{innerSize.x, headerHeight};
        const float radius =
            std::clamp(visual.style.cornerRadius - borderThickness, 2.0F, std::min(node.size.x, node.size.y) * 0.22F);

        const glm::vec4 headerLeft = mixColor(visual.headerColor, visual.style.bodyColor, 0.28F);
        const glm::vec4 headerRight = mixColor(visual.headerAccentColor, visual.style.bodyColor, 0.38F);
        appendTopRoundedGradientRect(geometry, headerPosition, headerSize, radius, headerLeft, headerRight);

        const float iconBoxExtent = std::min(24.0F, std::max(14.0F, headerHeight - 12.0F));
        const glm::vec2 iconBoxSize{iconBoxExtent, iconBoxExtent};
        const glm::vec2 iconBoxPosition = headerPosition + glm::vec2{7.0F, (headerHeight - iconBoxExtent) * 0.5F};
        appendRoundedGradientRect(geometry, iconBoxPosition, iconBoxSize, 4.0F, brighten(headerLeft, 0.14F),
                                  brighten(headerRight, 0.14F));
        appendIconShape(geometry, visual.iconShape, iconBoxPosition, iconBoxSize, visual.iconColor);

        const float textLeft = iconBoxPosition.x + iconBoxSize.x + 8.0F;
        const glm::vec2 textPosition{textLeft, headerPosition.y + 7.0F};
        const glm::vec2 textSize{std::max(1.0F, headerPosition.x + headerSize.x - textLeft - 22.0F),
                                 std::max(1.0F, headerHeight - 12.0F)};
        if (visual.subtitle.empty()) {
            appendText(text, visual.title, textPosition, textSize, m_fontOptions.titleSize, visual.textColor);
        } else {
            const float titleLineHeight = std::min(22.0F, textSize.y * 0.58F);
            appendText(text, visual.title, textPosition + glm::vec2{0.0F, textSize.y - titleLineHeight},
                       {textSize.x, titleLineHeight}, m_fontOptions.titleSize, visual.textColor);
            appendText(text, visual.subtitle, textPosition, {textSize.x, textSize.y - titleLineHeight + 2.0F},
                       m_fontOptions.iconSize, {0.72F, 0.82F, 0.92F, 1.0F});
        }
    }

    const glm::mat4 vp = camera.viewProjection();
    if (!geometry.empty()) {
        glUseProgram(m_program);
        glUniformMatrix4fv(glGetUniformLocation(m_program, "uVP"), 1, GL_FALSE, &vp[0][0]);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(geometry.size() * sizeof(Vertex)), geometry.data(),
                     GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(geometry.size()));
    }

    const FontMetrics metrics = m_textRenderer.fontMetrics();
    for (const TextRun& run : text) {
        const float scale = run.fontSize / static_cast<float>(metrics.pixelSize);
        const float baselineY =
            run.position.y + (run.size.y - (metrics.ascent - metrics.descent) * scale) * 0.5F - metrics.descent * scale;
        m_textRenderer.render(run.value, {run.position.x, baselineY}, run.color, vp, ClipRect{run.position, run.size},
                              scale);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace ScopeCanvas::Widget::Render

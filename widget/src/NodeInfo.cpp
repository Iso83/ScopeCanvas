#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/render/geometry/RoundedRect.h>
#include <ScopeCanvas/widget/NodeInfo.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <utility>

namespace ScopeCanvas::Widget {
using namespace ScopeCanvas::Render;

namespace {
struct Vertex {
    float x;
    float y;
    float r;
    float g;
    float b;
    float a;
};

struct TextVertex {
    float x;
    float y;
    float u;
    float v;
    float r;
    float g;
    float b;
    float a;
};

struct TextQuad {
    unsigned int texture{0};
    std::array<TextVertex, 6> vertices{};
};

unsigned int compile(unsigned int type, const char* source) {
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

std::string defaultFontPath() {
#if defined(_WIN32)
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

TextQuad textQuad(unsigned int texture, glm::vec2 position, glm::vec2 size, glm::vec4 color,
                  glm::vec2 uv0 = {0.0F, 1.0F}, glm::vec2 uv1 = {1.0F, 0.0F}) {
    return {texture,
            {{{position.x, position.y + size.y, uv0.x, uv0.y, color.r, color.g, color.b, color.a},
              {position.x, position.y, uv0.x, uv1.y, color.r, color.g, color.b, color.a},
              {position.x + size.x, position.y, uv1.x, uv1.y, color.r, color.g, color.b, color.a},
              {position.x, position.y + size.y, uv0.x, uv0.y, color.r, color.g, color.b, color.a},
              {position.x + size.x, position.y, uv1.x, uv1.y, color.r, color.g, color.b, color.a},
              {position.x + size.x, position.y + size.y, uv1.x, uv0.y, color.r, color.g, color.b, color.a}}}};
}

bool appendClippedTextQuad(std::vector<TextQuad>& vertices, unsigned int texture, glm::vec2 position, glm::vec2 size,
                           glm::vec4 color, glm::vec2 clipPosition, glm::vec2 clipSize) {
    const glm::vec2 minPoint{std::max(position.x, clipPosition.x), std::max(position.y, clipPosition.y)};
    const glm::vec2 maxPoint{std::min(position.x + size.x, clipPosition.x + clipSize.x),
                             std::min(position.y + size.y, clipPosition.y + clipSize.y)};
    if (maxPoint.x <= minPoint.x || maxPoint.y <= minPoint.y)
        return false;

    const glm::vec2 uv0{(minPoint.x - position.x) / size.x, 1.0F - (maxPoint.y - position.y) / size.y};
    const glm::vec2 uv1{(maxPoint.x - position.x) / size.x, 1.0F - (minPoint.y - position.y) / size.y};
    vertices.push_back(textQuad(texture, minPoint, maxPoint - minPoint, color, uv0, uv1));
    return true;
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

    glGenVertexArrays(1, &m_textVao);
    glGenBuffers(1, &m_textVbo);
    glBindVertexArray(m_textVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_textVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), reinterpret_cast<void*>(4 * sizeof(float)));
    glBindVertexArray(0);

    const char* geometryVs = R"(#version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec4 aColor;
        uniform mat4 uVP;
        out vec4 vColor;
        void main(){ gl_Position = uVP * vec4(aPos, 0.0, 1.0); vColor = aColor; })";
    const char* geometryFs = R"(#version 330 core
        in vec4 vColor;
        out vec4 FragColor;
        void main(){ FragColor = vColor; })";
    const unsigned int gv = compile(GL_VERTEX_SHADER, geometryVs);
    const unsigned int gf = compile(GL_FRAGMENT_SHADER, geometryFs);
    m_program = glCreateProgram();
    glAttachShader(m_program, gv);
    glAttachShader(m_program, gf);
    glLinkProgram(m_program);
    glDeleteShader(gv);
    glDeleteShader(gf);

    const char* textVs = R"(#version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUv;
        layout(location = 2) in vec4 aColor;
        uniform mat4 uVP;
        out vec2 vUv;
        out vec4 vColor;
        void main(){ gl_Position = uVP * vec4(aPos, 0.0, 1.0); vUv = aUv; vColor = aColor; })";
    const char* textFs = R"(#version 330 core
        in vec2 vUv;
        in vec4 vColor;
        uniform sampler2D uGlyph;
        out vec4 FragColor;
        void main(){ FragColor = vec4(vColor.rgb, vColor.a * texture(uGlyph, vUv).r); })";
    const unsigned int tv = compile(GL_VERTEX_SHADER, textVs);
    const unsigned int tf = compile(GL_FRAGMENT_SHADER, textFs);
    m_textProgram = glCreateProgram();
    glAttachShader(m_textProgram, tv);
    glAttachShader(m_textProgram, tf);
    glLinkProgram(m_textProgram);
    glDeleteShader(tv);
    glDeleteShader(tf);

    return loadFont();
}

void NodeInfoRenderer::shutdown() {
    releaseFont();
    if (m_textProgram != 0)
        glDeleteProgram(m_textProgram);
    if (m_textVbo != 0)
        glDeleteBuffers(1, &m_textVbo);
    if (m_textVao != 0)
        glDeleteVertexArrays(1, &m_textVao);
    if (m_program != 0)
        glDeleteProgram(m_program);
    if (m_vbo != 0)
        glDeleteBuffers(1, &m_vbo);
    if (m_vao != 0)
        glDeleteVertexArrays(1, &m_vao);
    m_textProgram = m_textVbo = m_textVao = m_program = m_vbo = m_vao = 0;
}

bool NodeInfoRenderer::loadFont() {
    FT_Library library = nullptr;
    if (FT_Init_FreeType(&library) != 0)
        return false;

    FT_Face face = nullptr;
    if (FT_New_Face(library, m_fontOptions.fontPath.c_str(), 0, &face) != 0) {
        FT_Done_FreeType(library);
        return false;
    }

    const float atlasScale = std::max(m_fontOptions.atlasScale, 1.0F);
    const auto atlasPixelSize = static_cast<unsigned int>(
        std::ceil(std::max(m_fontOptions.titleSize, m_fontOptions.iconSize) * atlasScale));
    FT_Set_Pixel_Sizes(face, 0, atlasPixelSize);
    m_fontAtlasSize = static_cast<float>(atlasPixelSize);
    m_fontAscent = static_cast<float>(face->size->metrics.ascender >> 6);
    m_fontDescent = static_cast<float>(face->size->metrics.descender >> 6);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 32; c < 127; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            continue;

        unsigned int texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(face->glyph->bitmap.width),
                     static_cast<GLsizei>(face->glyph->bitmap.rows), 0, GL_RED, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_glyphs[static_cast<char>(c)] = {
            texture,
            {static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows)},
            {static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top)},
            static_cast<unsigned int>(face->glyph->advance.x)};
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return !m_glyphs.empty();
}

void NodeInfoRenderer::releaseFont() {
    for (auto& [_, glyph] : m_glyphs) {
        if (glyph.texture != 0)
            glDeleteTextures(1, &glyph.texture);
    }
    m_glyphs.clear();
}

void NodeInfoRenderer::render(const std::vector<Render::Scene::NodeRenderData>& nodes,
                              const Render::Camera::Camera2D& camera,
                              const Render::Theme::NodeVisualRegistry& registry) const {
    auto appendText = [this](std::vector<TextQuad>& textVertices, const std::string& value, glm::vec2 rectPosition,
                             glm::vec2 rectSize, float size, glm::vec4 color) {
        const float scale = size / m_fontAtlasSize;
        const float ascent = m_fontAscent * scale;
        const float descent = m_fontDescent * scale;
        const float baselineY = rectPosition.y + (rectSize.y - (ascent - descent)) * 0.5F - descent;
        float x = rectPosition.x;

        for (char c : value) {
            const auto it = m_glyphs.find(c);
            if (it == m_glyphs.end())
                continue;

            const GlyphInfo& glyph = it->second;
            const glm::vec2 glyphPosition{x + glyph.bearing.x * scale,
                                          baselineY - (glyph.size.y - glyph.bearing.y) * scale};
            const glm::vec2 glyphSize = glyph.size * scale;
            appendClippedTextQuad(textVertices, glyph.texture, glyphPosition, glyphSize, color, rectPosition, rectSize);
            x += static_cast<float>(glyph.advance >> 6U) * scale;
            if (x > rectPosition.x + rectSize.x)
                break;
        }
    };

    std::vector<Vertex> geometry;
    std::vector<TextQuad> text;
    geometry.reserve(nodes.size() * 128U);

    for (const Render::Scene::NodeRenderData& node : nodes) {
        const Render::Theme::NodeVisual& visual = registry.getVisual(node.typeId);
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

    if (!text.empty()) {
        glUseProgram(m_textProgram);
        glUniformMatrix4fv(glGetUniformLocation(m_textProgram, "uVP"), 1, GL_FALSE, &vp[0][0]);
        glUniform1i(glGetUniformLocation(m_textProgram, "uGlyph"), 0);
        glBindVertexArray(m_textVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_textVbo);
        for (const TextQuad& quad : text) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, quad.texture);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(quad.vertices.size() * sizeof(TextVertex)),
                         quad.vertices.data(), GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(quad.vertices.size()));
        }
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void NodeInfoRenderer::render(const std::vector<NodeInfo>& nodes, const Render::Camera::Camera2D& camera) const {
    auto appendText = [this](std::vector<TextQuad>& textVertices, const std::string& value, glm::vec2 rectPosition,
                             glm::vec2 rectSize, float size, glm::vec4 color) {
        const float scale = size / m_fontAtlasSize;
        const float ascent = m_fontAscent * scale;
        const float descent = m_fontDescent * scale;
        const float baselineY = rectPosition.y + (rectSize.y - (ascent - descent)) * 0.5F - descent;
        float x = rectPosition.x;

        for (char c : value) {
            const auto it = m_glyphs.find(c);
            if (it == m_glyphs.end())
                continue;

            const GlyphInfo& glyph = it->second;
            const glm::vec2 glyphPosition{x + glyph.bearing.x * scale,
                                          baselineY - (glyph.size.y - glyph.bearing.y) * scale};
            const glm::vec2 glyphSize = glyph.size * scale;
            appendClippedTextQuad(textVertices, glyph.texture, glyphPosition, glyphSize, color, rectPosition, rectSize);
            x += static_cast<float>(glyph.advance >> 6U) * scale;
            if (x > rectPosition.x + rectSize.x)
                break;
        }
    };

    std::vector<Vertex> geometry;
    std::vector<TextQuad> text;
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

    if (!text.empty()) {
        glUseProgram(m_textProgram);
        glUniformMatrix4fv(glGetUniformLocation(m_textProgram, "uVP"), 1, GL_FALSE, &vp[0][0]);
        glUniform1i(glGetUniformLocation(m_textProgram, "uGlyph"), 0);
        glBindVertexArray(m_textVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_textVbo);
        for (const TextQuad& quad : text) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, quad.texture);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(quad.vertices.size() * sizeof(TextVertex)),
                         quad.vertices.data(), GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(quad.vertices.size()));
        }
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace ScopeCanvas::Widget

#include <array>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <ScopeCanvas/engine/render/gl/OpenGLApi.h>
#include <ScopeCanvas/engine/render/gl/Shader.h>
#include <ScopeCanvas/engine/render/gl/ShaderSource.h>
#include <ScopeCanvas/engine/render/text/Renderer.h>
#include <unordered_map>

namespace ScopeCanvas::Engine::Render::Text {
struct TextVertex {
    float x, y, u, v, r, g, b, a;
};

struct GlyphInfo {
    unsigned int texture{};
    glm::vec2 size{};
    glm::vec2 bearing{};
    unsigned int advance{};
};

std::string vertexSource() {
    return std::string(GL::ShaderVersionPrefix) + R"(
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUv;
        layout(location = 2) in vec4 aColor;
        uniform mat4 uVP;
        out vec2 vUv;
        out vec4 vColor;
        void main() { gl_Position = uVP * vec4(aPos, 0.0, 1.0); vUv = aUv; vColor = aColor; }
    )";
}

std::string fragmentSource() {
    return std::string(GL::ShaderVersionPrefix) + R"(
        in vec2 vUv;
        in vec4 vColor;
        uniform sampler2D uGlyph;
        out vec4 FragColor;
        void main() { FragColor = vec4(vColor.rgb, vColor.a * texture(uGlyph, vUv).r); }
    )";
}

struct Renderer::Impl {
    unsigned int vao{};
    unsigned int vbo{};
    GL::Shader shader{};
    std::unordered_map<unsigned char, GlyphInfo> glyphs{};
    FontMetrics metrics{};
};

Renderer::Renderer() : m_impl(std::make_unique<Impl>()) {}
Renderer::~Renderer() {
    if (m_impl)
        shutdown();
}
Renderer::Renderer(Renderer&&) noexcept = default;
Renderer& Renderer::operator=(Renderer&&) noexcept = default;

bool Renderer::init() {
    if (m_impl->vao != 0)
        return true;

    glGenVertexArrays(1, &m_impl->vao);
    glGenBuffers(1, &m_impl->vbo);
    glBindVertexArray(m_impl->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_impl->vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), reinterpret_cast<void*>(4 * sizeof(float)));
    glBindVertexArray(0);
    const std::string vs = vertexSource();
    const std::string fs = fragmentSource();
    if (m_impl->vao == 0 || m_impl->vbo == 0 || !m_impl->shader.load(vs.c_str(), fs.c_str())) {
        shutdown();
        return false;
    }

    return true;
}

void Renderer::shutdown() {
    if (!m_impl)
        return;

    releaseFont();
    m_impl->shader = {};
    if (m_impl->vbo != 0)
        glDeleteBuffers(1, &m_impl->vbo);
    if (m_impl->vao != 0)
        glDeleteVertexArrays(1, &m_impl->vao);
    m_impl->vbo = m_impl->vao = 0;
}

bool Renderer::loadFont(const std::string& path, unsigned int pixelSize) {
    releaseFont();
    if (m_impl->vao == 0 || path.empty() || pixelSize == 0)
        return false;

    FT_Library library{};
    if (FT_Init_FreeType(&library) != 0)
        return false;

    FT_Face face{};
    if (FT_New_Face(library, path.c_str(), 0, &face) != 0) {
        FT_Done_FreeType(library);
        return false;
    }

    if (FT_Set_Pixel_Sizes(face, 0, pixelSize) != 0) {
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return false;
    }

    m_impl->metrics = {static_cast<float>(face->size->metrics.ascender >> 6),
                       static_cast<float>(face->size->metrics.descender >> 6),
                       static_cast<float>(face->size->metrics.height >> 6), pixelSize};
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned int c = 32; c < 256; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            continue;

        unsigned int texture{};
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, static_cast<GLsizei>(face->glyph->bitmap.width),
                     static_cast<GLsizei>(face->glyph->bitmap.rows), 0, GL_RED, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_impl->glyphs[static_cast<unsigned char>(c)] = {texture,
                                                         {face->glyph->bitmap.width, face->glyph->bitmap.rows},
                                                         {face->glyph->bitmap_left, face->glyph->bitmap_top},
                                                         static_cast<unsigned int>(face->glyph->advance.x)};
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    if (m_impl->glyphs.empty()) {
        releaseFont();
        return false;
    }

    return true;
}

void Renderer::releaseFont() {
    for (auto& [_, glyph] : m_impl->glyphs)
        if (glyph.texture != 0)
            glDeleteTextures(1, &glyph.texture);
    m_impl->glyphs.clear();
    m_impl->metrics = {};
}

bool Renderer::ready() const {
    return m_impl->vao != 0 && m_impl->shader.id() != 0 && !m_impl->glyphs.empty();
}
FontMetrics Renderer::fontMetrics() const {
    return m_impl->metrics;
}
glm::vec2 Renderer::measure(std::string_view text, float worldScale) const {
    if (text.empty())
        return {};
    float width{};
    for (unsigned char c : text) {
        const auto it = m_impl->glyphs.find(c);
        if (it != m_impl->glyphs.end())
            width += static_cast<float>(it->second.advance >> 6U) * worldScale;
    }
    return {width, (m_impl->metrics.ascent - m_impl->metrics.descent) * worldScale};
}

void Renderer::render(std::string_view text, glm::vec2 baseline, glm::vec4 color, const glm::mat4& viewProjection,
                      std::optional<ClipRect> clip, float worldScale) const {
    if (!ready() || worldScale <= 0.0F)
        return;
    m_impl->shader.use();
    glUniformMatrix4fv(glGetUniformLocation(m_impl->shader.id(), "uVP"), 1, GL_FALSE, &viewProjection[0][0]);
    glUniform1i(glGetUniformLocation(m_impl->shader.id(), "uGlyph"), 0);
    glBindVertexArray(m_impl->vao);
    float x = baseline.x;
    for (unsigned char c : text) {
        const auto it = m_impl->glyphs.find(c);
        if (it == m_impl->glyphs.end())
            continue;
        const GlyphInfo& glyph = it->second;
        glm::vec2 position{x + glyph.bearing.x * worldScale,
                           baseline.y - (glyph.size.y - glyph.bearing.y) * worldScale};
        glm::vec2 size = glyph.size * worldScale;
        glm::vec2 min = position;
        glm::vec2 max = position + size;
        if (clip) {
            min = glm::max(min, clip->position);
            max = glm::min(max, clip->position + clip->size);
        }
        if (max.x > min.x && max.y > min.y && size.x > 0.0F && size.y > 0.0F) {
            const glm::vec2 uv0{(min.x - position.x) / size.x, 1.0F - (max.y - position.y) / size.y};
            const glm::vec2 uv1{(max.x - position.x) / size.x, 1.0F - (min.y - position.y) / size.y};
            const std::array<TextVertex, 6> vertices{{
                {min.x, max.y, uv0.x, uv0.y, color.r, color.g, color.b, color.a},
                {min.x, min.y, uv0.x, uv1.y, color.r, color.g, color.b, color.a},
                {max.x, min.y, uv1.x, uv1.y, color.r, color.g, color.b, color.a},
                {min.x, max.y, uv0.x, uv0.y, color.r, color.g, color.b, color.a},
                {max.x, min.y, uv1.x, uv1.y, color.r, color.g, color.b, color.a},
                {max.x, max.y, uv1.x, uv0.y, color.r, color.g, color.b, color.a},
            }};
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, glyph.texture);
            glBindBuffer(GL_ARRAY_BUFFER, m_impl->vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
        }
        x += static_cast<float>(glyph.advance >> 6U) * worldScale;
        if (clip && x > clip->position.x + clip->size.x)
            break;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace ScopeCanvas::Engine::Render::Text

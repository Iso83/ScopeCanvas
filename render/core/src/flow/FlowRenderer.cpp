#include <ScopeCanvas/render/flow/FlowRenderer.h>
#include <ScopeCanvas/render/EdgeRenderer.h>
#include <ScopeCanvas/render/GridRenderer.h>
#include <ScopeCanvas/render/NodeRenderer.h>
#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/render/scene/RenderScene.h>
#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <unordered_map>

namespace ScopeCanvas::Render::Flow {
namespace {
using ScopeCanvas::Core::Ids::EdgeId;
using ScopeCanvas::Core::Ids::NodeId;
using ScopeCanvas::Core::Ids::NodeTypeId;
using ScopeCanvas::Core::Flow::FlowGroup;
using ScopeCanvas::Core::Flow::FlowGroupId;
using ScopeCanvas::Core::Flow::FlowStep;
using ScopeCanvas::Routing::Flow::FlowGroupLayout;
using ScopeCanvas::Routing::Flow::FlowLayoutResult;
using ScopeCanvas::Routing::Flow::FlowRowLayout;
using ScopeCanvas::Routing::Flow::FlowStepLayout;

struct GlyphInfo {
    unsigned int texture{};
    glm::vec2 size{};
    glm::vec2 bearing{};
    unsigned int advance{};
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

ScopeCanvas::Render::NodeRenderStyle stepStyle(NodeTypeId /*typeId*/) {
    ScopeCanvas::Render::NodeRenderStyle style{};
    style.bodyColor = {0.15F, 0.18F, 0.23F, 0.98F};
    style.borderColor = {0.72F, 0.80F, 0.92F, 1.0F};
    style.cornerRadius = 8.0F;
    style.borderThickness = 1.8F;
    return style;
}

const FlowGroup* findGroup(const Core::Flow::FlowDocument& document, FlowGroupId groupId) {
    for (const FlowGroup& group : document.groups())
        if (group.id == groupId)
            return &group;
    return nullptr;
}

const FlowStep* findStep(const Core::Flow::FlowDocument& document, NodeId stepId) {
    return document.getStep(stepId);
}
} // namespace

struct FlowRenderer::Impl {
    GridRenderer grid{};
    NodeRenderer nodes{};
    EdgeRenderer lines{};
    unsigned int textVao{};
    unsigned int textVbo{};
    unsigned int textProgram{};
    unsigned int rectVao{};
    unsigned int rectVbo{};
    unsigned int rectProgram{};
    std::unordered_map<char, GlyphInfo> glyphs{};
    float fontAtlasSize{1.0F};
    bool gridInitialized{false};
    bool nodesInitialized{false};
    bool linesInitialized{false};
    bool initialized{false};

    bool init();
    void shutdown();
    bool initText();
    bool initRect();
    void shutdownRect();
    void renderRect(glm::vec2 position, glm::vec2 size, glm::vec4 color,
                    const ScopeCanvas::Render::Camera::Camera2D& camera) const;
    void shutdownText();
    bool loadFont();
    void renderText(const std::string& text, glm::vec2 position, float size, glm::vec4 color,
                    const ScopeCanvas::Render::Camera::Camera2D& camera) const;
    void render(const Core::Flow::FlowDocument& document, const Routing::Flow::FlowLayoutResult& layout,
                const ScopeCanvas::Render::Camera::Camera2D& camera, const FlowRenderOptions& options) const;
    void renderParentContainers(const Routing::Flow::FlowLayoutResult& layout,
                                const ScopeCanvas::Render::Camera::Camera2D& camera) const;
    void renderGroupHeaders(const Core::Flow::FlowDocument& document, const Routing::Flow::FlowLayoutResult& layout,
                            const ScopeCanvas::Render::Camera::Camera2D& camera) const;
    void renderRails(const Routing::Flow::FlowLayoutResult& layout,
                     const ScopeCanvas::Render::Camera::Camera2D& camera) const;
    void renderStepNodes(const Core::Flow::FlowDocument& document, const Routing::Flow::FlowLayoutResult& layout,
                         const ScopeCanvas::Render::Camera::Camera2D& camera, const FlowRenderOptions& options) const;
};

FlowGroupHeaderGeometry groupHeaderGeometry(const FlowGroupLayout& group, float visibleCenterX, float labelWidth) {
    constexpr float titleInset = 52.0F;
    const float labelHalfWidth = labelWidth * 0.5F;
    const float minX = group.boundsPosition.x + titleInset;
    const float maxX = group.boundsPosition.x + group.boundsSize.x - titleInset - labelWidth;
    const float desiredX = visibleCenterX - labelHalfWidth;
    const float textX = minX > maxX ? group.boundsPosition.x + (group.boundsSize.x - labelWidth) * 0.5F
                                    : std::clamp(desiredX, minX, maxX);
    const float headerBottom = group.boundsPosition.y + group.boundsSize.y - group.headerSize.y;
    return {{textX, headerBottom - 6.0F}, {textX - 10.0F, headerBottom + 2.0F},
            {labelWidth + 20.0F, std::max(group.headerSize.y - 4.0F, 1.0F)}};
}

FlowRenderer::FlowRenderer() : m_impl(std::make_unique<Impl>()) {}

FlowRenderer::~FlowRenderer() { shutdown(); }

bool FlowRenderer::init() { return m_impl->init(); }

void FlowRenderer::shutdown() { m_impl->shutdown(); }

bool FlowRenderer::Impl::init() {
    if (initialized)
        return true;
    shutdown();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gridInitialized = grid.init();
    nodesInitialized = gridInitialized && nodes.init();
    linesInitialized = nodesInitialized && lines.init();
    initialized = linesInitialized && initText() && initRect();
    if (!initialized)
        shutdown();
    return initialized;
}

void FlowRenderer::Impl::shutdown() {
    shutdownRect();
    shutdownText();
    if (linesInitialized)
        lines.shutdown();
    if (nodesInitialized)
        nodes.shutdown();
    if (gridInitialized)
        grid.shutdown();
    linesInitialized = false;
    nodesInitialized = false;
    gridInitialized = false;
    initialized = false;
}

bool FlowRenderer::Impl::initText() {
    const char* vs = R"(#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec4 aColor;
uniform mat4 uVP;
out vec2 vUv;
out vec4 vColor;
void main() {
    gl_Position = uVP * vec4(aPos, 0.0, 1.0);
    vUv = aUv;
    vColor = aColor;
})";

    const char* fs = R"(#version 330 core
in vec2 vUv;
in vec4 vColor;
uniform sampler2D uGlyph;
out vec4 FragColor;
void main() {
    FragColor = vec4(vColor.rgb, vColor.a * texture(uGlyph, vUv).r);
})";

    const unsigned int vertexShader = compile(GL_VERTEX_SHADER, vs);
    const unsigned int fragmentShader = compile(GL_FRAGMENT_SHADER, fs);
    textProgram = glCreateProgram();
    glAttachShader(textProgram, vertexShader);
    glAttachShader(textProgram, fragmentShader);
    glLinkProgram(textProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &textVao);
    glGenBuffers(1, &textVbo);
    glBindVertexArray(textVao);
    glBindBuffer(GL_ARRAY_BUFFER, textVbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), reinterpret_cast<void*>(4 * sizeof(float)));
    glBindVertexArray(0);

    return loadFont();
}

bool FlowRenderer::Impl::initRect() {
    const char* vs = R"(#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;
uniform mat4 uVP;
out vec4 vColor;
void main() {
    gl_Position = uVP * vec4(aPos, 0.0, 1.0);
    vColor = aColor;
})";
    const char* fs = R"(#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main() { FragColor = vColor; })";
    const unsigned int vertexShader = compile(GL_VERTEX_SHADER, vs);
    const unsigned int fragmentShader = compile(GL_FRAGMENT_SHADER, fs);
    rectProgram = glCreateProgram();
    glAttachShader(rectProgram, vertexShader);
    glAttachShader(rectProgram, fragmentShader);
    glLinkProgram(rectProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &rectVao);
    glGenBuffers(1, &rectVbo);
    glBindVertexArray(rectVao);
    glBindBuffer(GL_ARRAY_BUFFER, rectVbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glBindVertexArray(0);
    return rectProgram != 0;
}

void FlowRenderer::Impl::shutdownRect() {
    if (rectProgram != 0)
        glDeleteProgram(rectProgram);
    if (rectVbo != 0)
        glDeleteBuffers(1, &rectVbo);
    if (rectVao != 0)
        glDeleteVertexArrays(1, &rectVao);
    rectProgram = rectVbo = rectVao = 0;
}

void FlowRenderer::Impl::renderRect(glm::vec2 position, glm::vec2 size, glm::vec4 color,
                              const ScopeCanvas::Render::Camera::Camera2D& camera) const {
    const float x0 = position.x;
    const float y0 = position.y;
    const float x1 = position.x + size.x;
    const float y1 = position.y + size.y;
    const float vertices[] = {
        x0, y0, color.r, color.g, color.b, color.a, x1, y0, color.r, color.g, color.b, color.a,
        x1, y1, color.r, color.g, color.b, color.a, x0, y0, color.r, color.g, color.b, color.a,
        x1, y1, color.r, color.g, color.b, color.a, x0, y1, color.r, color.g, color.b, color.a,
    };
    glUseProgram(rectProgram);
    const glm::mat4 vp = camera.viewProjection();
    glUniformMatrix4fv(glGetUniformLocation(rectProgram, "uVP"), 1, GL_FALSE, &vp[0][0]);
    glBindVertexArray(rectVao);
    glBindBuffer(GL_ARRAY_BUFFER, rectVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void FlowRenderer::Impl::shutdownText() {
    for (auto& [_, glyph] : glyphs) {
        if (glyph.texture != 0)
            glDeleteTextures(1, &glyph.texture);
    }
    glyphs.clear();
    if (textProgram != 0)
        glDeleteProgram(textProgram);
    if (textVbo != 0)
        glDeleteBuffers(1, &textVbo);
    if (textVao != 0)
        glDeleteVertexArrays(1, &textVao);
    textProgram = textVbo = textVao = 0;
}

bool FlowRenderer::Impl::loadFont() {
    FT_Library library = nullptr;
    if (FT_Init_FreeType(&library) != 0)
        return false;

    FT_Face face = nullptr;
    if (FT_New_Face(library, defaultFontPath().c_str(), 0, &face) != 0) {
        FT_Done_FreeType(library);
        return false;
    }

    constexpr float atlasSize = 192.0F;
    FT_Set_Pixel_Sizes(face, 0, static_cast<unsigned int>(atlasSize));
    fontAtlasSize = atlasSize;

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
        glyphs[static_cast<char>(c)] = {texture,
                                          {static_cast<float>(face->glyph->bitmap.width),
                                           static_cast<float>(face->glyph->bitmap.rows)},
                                          {static_cast<float>(face->glyph->bitmap_left),
                                           static_cast<float>(face->glyph->bitmap_top)},
                                          static_cast<unsigned int>(face->glyph->advance.x)};
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return !glyphs.empty();
}

void FlowRenderer::Impl::renderText(const std::string& text, glm::vec2 position, float size, glm::vec4 color,
                              const ScopeCanvas::Render::Camera::Camera2D& camera) const {
    const float scale = size / fontAtlasSize;
    const float baselineY = position.y;
    float x = position.x;
    std::vector<TextVertex> vertices;
    vertices.reserve(text.size() * 6U);

    for (char c : text) {
        const auto it = glyphs.find(c);
        if (it == glyphs.end())
            continue;
        const GlyphInfo& glyph = it->second;
        const glm::vec2 glyphPosition{x + glyph.bearing.x * scale,
                                      baselineY - (glyph.size.y - glyph.bearing.y) * scale};
        const glm::vec2 glyphSize = glyph.size * scale;
        const float x0 = glyphPosition.x;
        const float y0 = glyphPosition.y;
        const float x1 = glyphPosition.x + glyphSize.x;
        const float y1 = glyphPosition.y + glyphSize.y;
        vertices.push_back({x0, y0, 0.0F, 1.0F, color.r, color.g, color.b, color.a});
        vertices.push_back({x1, y0, 1.0F, 1.0F, color.r, color.g, color.b, color.a});
        vertices.push_back({x1, y1, 1.0F, 0.0F, color.r, color.g, color.b, color.a});
        vertices.push_back({x0, y0, 0.0F, 1.0F, color.r, color.g, color.b, color.a});
        vertices.push_back({x1, y1, 1.0F, 0.0F, color.r, color.g, color.b, color.a});
        vertices.push_back({x0, y1, 0.0F, 0.0F, color.r, color.g, color.b, color.a});
        x += static_cast<float>(glyph.advance >> 6U) * scale;
    }

    if (vertices.empty())
        return;

    glUseProgram(textProgram);
    const glm::mat4 vp = camera.viewProjection();
    glUniformMatrix4fv(glGetUniformLocation(textProgram, "uVP"), 1, GL_FALSE, &vp[0][0]);
    glUniform1i(glGetUniformLocation(textProgram, "uGlyph"), 0);
    glBindVertexArray(textVao);
    glBindBuffer(GL_ARRAY_BUFFER, textVbo);

    std::size_t vertexOffset = 0;
    for (char c : text) {
        const auto it = glyphs.find(c);
        if (it == glyphs.end())
            continue;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, it->second.texture);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(6U * sizeof(TextVertex)), vertices.data() + vertexOffset,
                     GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        vertexOffset += 6U;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FlowRenderer::render(const Core::Flow::FlowDocument& document, const FlowLayoutResult& layout,
                          const ScopeCanvas::Render::Camera::Camera2D& camera, const FlowRenderOptions& options) const {
    m_impl->render(document, layout, camera, options);
}

void FlowRenderer::Impl::render(const Core::Flow::FlowDocument& document, const FlowLayoutResult& layout,
                                const ScopeCanvas::Render::Camera::Camera2D& camera,
                                const FlowRenderOptions& options) const {
    if (!initialized)
        return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.07F, 0.08F, 0.10F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (options.showGrid)
        grid.render(camera, 32.0F);

    renderParentContainers(layout, camera);
    renderGroupHeaders(document, layout, camera);
    renderRails(layout, camera);
    renderStepNodes(document, layout, camera, options);
}


void FlowRenderer::Impl::renderParentContainers(const FlowLayoutResult& layout,
                                          const ScopeCanvas::Render::Camera::Camera2D& camera) const {
    std::vector<ScopeCanvas::Render::Scene::NodeRenderData> containers;
    for (const FlowGroupLayout& group : layout.groups) {
        containers.push_back({NodeId{group.groupId.value()}, NodeTypeId{83}, group.boundsPosition, group.boundsSize, 0});
    }
    for (const FlowStepLayout& step : layout.steps) {
        if (!step.hasChildren || step.collapsed)
            continue;
        containers.push_back({step.stepId, NodeTypeId{80}, step.subtreePosition, step.subtreeSize, 0});
    }
    nodes.render(containers, camera, {}, [](NodeTypeId typeId) {
        ScopeCanvas::Render::NodeRenderStyle style{};
        if (typeId.value() == 83U) {
            style.bodyColor = {0.07F, 0.10F, 0.15F, 0.58F};
            style.borderColor = {0.34F, 0.52F, 0.78F, 0.96F};
            style.cornerRadius = 10.0F;
            style.borderThickness = 1.5F;
            return style;
        }
        style.bodyColor = {0.10F, 0.13F, 0.18F, 0.10F};
        style.borderColor = {0.24F, 0.36F, 0.54F, 0.42F};
        style.cornerRadius = 9.0F;
        style.borderThickness = 1.0F;
        return style;
    });
}

void FlowRenderer::Impl::renderGroupHeaders(const Core::Flow::FlowDocument& document, const FlowLayoutResult& layout,
                                      const ScopeCanvas::Render::Camera::Camera2D& camera) const {
    const float visibleCenterX = camera.position().x;

    std::vector<ScopeCanvas::Render::Scene::EdgeRenderData> dividers;
    struct HeaderText {
        const FlowGroup* group{};
        glm::vec2 position{};
    };
    std::vector<HeaderText> headerTexts;
    std::uint32_t lineId = 7000;

    for (const FlowGroupLayout& groupLayout : layout.groups) {
        const FlowGroup* group = findGroup(document, groupLayout.groupId);
        if (group == nullptr)
            continue;

        const float headerHeight = groupLayout.headerSize.y;
        const float headerBottom = groupLayout.boundsPosition.y + groupLayout.boundsSize.y - headerHeight;

        if (!group->collapsed) {
            constexpr float cornerInset = 12.0F;
            const glm::vec2 headerPosition{groupLayout.boundsPosition.x + cornerInset, headerBottom};
            const glm::vec2 headerSize{std::max(groupLayout.boundsSize.x - cornerInset * 2.0F, 1.0F),
                                       std::max(headerHeight - 1.0F, 1.0F)};
            renderRect(headerPosition, headerSize, {0.08F, 0.12F, 0.18F, 0.88F}, camera);
            dividers.push_back({EdgeId{lineId++}, {{groupLayout.boundsPosition.x, headerBottom},
                                                   {groupLayout.boundsPosition.x + groupLayout.boundsSize.x, headerBottom}}});
        }

        headerTexts.push_back({group, groupHeaderGeometry(groupLayout, visibleCenterX).textPosition});
    }

    lines.render(dividers, camera);

    for (const HeaderText& header : headerTexts) {
        const std::string title = std::string(header.group->collapsed ? "[+] " : "[-] ") + header.group->title;
        renderText(title, header.position + glm::vec2{0.0F, 28.0F}, 17.0F, {0.92F, 0.95F, 1.0F, 1.0F}, camera);
        if (!header.group->summary.empty())
            renderText(header.group->summary, header.position + glm::vec2{0.0F, 10.0F}, 11.0F,
                       {0.72F, 0.82F, 0.94F, 1.0F}, camera);
    }
}

void FlowRenderer::Impl::renderRails(const FlowLayoutResult& layout,
                                    const ScopeCanvas::Render::Camera::Camera2D& camera) const {
    std::vector<ScopeCanvas::Render::Scene::EdgeRenderData> lineSegments;
    std::uint32_t lineId = 1;

    for (const FlowRowLayout& row : layout.rows) {
        std::vector<const FlowStepLayout*> rowSteps;
        for (const FlowStepLayout& step : layout.steps)
            if (step.rowId == row.rowId && step.depth == 0U)
                rowSteps.push_back(&step);
        std::sort(rowSteps.begin(), rowSteps.end(), [](const FlowStepLayout* a, const FlowStepLayout* b) {
            return a->position.x < b->position.x;
        });
        if (rowSteps.empty())
            continue;

        if (rowSteps.size() < 2U)
            continue;

        for (std::size_t i = 0; i + 1U < rowSteps.size(); ++i) {
            const FlowStepLayout* current = rowSteps[i];
            const FlowStepLayout* next = rowSteps[i + 1U];
            const glm::vec2 right{current->position.x + current->size.x, current->railAnchor.y};
            const glm::vec2 left{next->position.x, next->railAnchor.y};
            lineSegments.push_back({EdgeId{lineId++}, {right, left}});
        }
    }

    for (const FlowStepLayout& child : layout.steps) {
        if (!child.parentStepId.isValid())
            continue;
        const FlowStepLayout* parent = layout.step(child.parentStepId);
        if (parent == nullptr || parent->collapsed)
            continue;
        const glm::vec2 parentPoint{parent->position.x + parent->size.x * 0.5F, parent->position.y};
        const glm::vec2 childPoint{child.position.x + child.size.x * 0.5F, child.position.y + child.size.y};
        const glm::vec2 elbow{childPoint.x, parentPoint.y - 22.0F};
        lineSegments.push_back({EdgeId{lineId++}, {parentPoint, {parentPoint.x, elbow.y}, elbow, childPoint}});
    }

    lines.render(lineSegments, camera);
}

void FlowRenderer::Impl::renderStepNodes(const Core::Flow::FlowDocument& document, const FlowLayoutResult& layout,
                                   const ScopeCanvas::Render::Camera::Camera2D& camera,
                                   const FlowRenderOptions& options) const {
    std::vector<ScopeCanvas::Render::Scene::NodeRenderData> nodeParts;
    for (const FlowStepLayout& step : layout.steps) {
        const FlowStep* documentStep = findStep(document, step.stepId);
        nodeParts.push_back(
            {step.stepId, documentStep != nullptr ? documentStep->typeId : NodeTypeId{1}, step.position, step.size, 0});
    }

    const std::vector<NodeId> selected = options.selectedStep.isValid() ? std::vector<NodeId>{options.selectedStep}
                                                                        : std::vector<NodeId>{};
    nodes.render(nodeParts, camera, selected, [](NodeTypeId typeId) { return stepStyle(typeId); });

    for (const FlowStepLayout& stepLayout : layout.steps) {
        const FlowStep* step = findStep(document, stepLayout.stepId);
        if (step == nullptr)
            continue;
        const std::string prefix = stepLayout.hasChildren ? (stepLayout.collapsed ? "+ " : "- ") : "";
        renderText(prefix + step->title, stepLayout.position + glm::vec2{16.0F, 52.0F}, 15.0F,
                   {0.96F, 0.98F, 1.0F, 1.0F}, camera);
        renderText(step->description, stepLayout.position + glm::vec2{16.0F, 31.0F}, 12.0F,
                   {0.76F, 0.84F, 0.94F, 1.0F}, camera);
        if (!step->status.empty())
            renderText(step->status, stepLayout.position + glm::vec2{16.0F, 13.0F}, 11.0F,
                       {0.58F, 0.92F, 0.68F, 1.0F}, camera);
    }
}

} // namespace ScopeCanvas::Render::Flow

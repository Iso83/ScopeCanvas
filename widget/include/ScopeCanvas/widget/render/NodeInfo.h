#pragma once

#include <ScopeCanvas/render/NodeRenderer.h>
#include <ScopeCanvas/render/scene/RenderScene.h>
#include <ScopeCanvas/widget/render/theme/NodeVisualRegistry.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Widget::Render {

struct NodeInfo {
    Core::Ids::NodeId id{};
    Core::Ids::NodeTypeId typeId{};
    glm::vec2 position{};
    glm::vec2 size{};
    Render::Theme::NodeVisual visual{};
};

struct NodeInfoFontOptions {
    std::string fontPath{};
    float titleSize{13.0F};
    float iconSize{12.0F};
    float atlasScale{4.0F};
};

class NodeInfoRenderer {
  private:
    struct GlyphInfo {
        unsigned int texture{0};
        glm::vec2 size{};
        glm::vec2 bearing{};
        unsigned int advance{0};
    };

    unsigned int m_vao{0};
    unsigned int m_vbo{0};
    unsigned int m_program{0};
    unsigned int m_textVao{0};
    unsigned int m_textVbo{0};
    unsigned int m_textProgram{0};
    std::unordered_map<char, GlyphInfo> m_glyphs{};
    NodeInfoFontOptions m_fontOptions{};
    float m_fontAtlasSize{1.0F};
    float m_fontAscent{0.0F};
    float m_fontDescent{0.0F};

  public:
    explicit NodeInfoRenderer(NodeInfoFontOptions fontOptions = {});
    bool init();
    void shutdown();
    void render(const std::vector<ScopeCanvas::Render::Scene::NodeRenderData>& nodes,
                const ScopeCanvas::Render::Camera::Camera2D& camera,
                const Render::Theme::NodeVisualRegistry& registry) const;
    void render(const std::vector<NodeInfo>& nodes, const ScopeCanvas::Render::Camera::Camera2D& camera) const;

  private:
    bool loadFont();
    void releaseFont();
};

} // namespace ScopeCanvas::Widget::Render

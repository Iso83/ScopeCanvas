#pragma once

#include <ScopeCanvas/engine/render/NodeRenderer.h>
#include <ScopeCanvas/engine/render/scene/RenderScene.h>
#include <ScopeCanvas/engine/render/text/Renderer.h>
#include <ScopeCanvas/widget/render/theme/NodeVisualRegistry.h>
#include <string>
#include <vector>

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Widget::Render {

struct NodeInfo {
    Engine::Core::Ids::NodeId id{};
    Engine::Core::Ids::NodeTypeId typeId{};
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
    unsigned int m_vao{0};
    unsigned int m_vbo{0};
    unsigned int m_program{0};
    Engine::Render::Text::Renderer m_textRenderer{};
    NodeInfoFontOptions m_fontOptions{};

public:
    explicit NodeInfoRenderer(NodeInfoFontOptions fontOptions = {});
    bool init();
    void shutdown();
    void render(const std::vector<Engine::Render::Scene::NodeRenderData>& nodes,
                const Engine::Render::Camera::Camera2D& camera, const Theme::NodeVisualRegistry& registry) const;
    void render(const std::vector<NodeInfo>& nodes, const Engine::Render::Camera::Camera2D& camera) const;
};

} // namespace ScopeCanvas::Widget::Render

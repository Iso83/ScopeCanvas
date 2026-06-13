#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/TypeIds.h>
#include <ScopeCanvas/render/scene/RenderScene.h>
#include <functional>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Render::Renderers {
struct NodeRenderStyle {
    glm::vec4 bodyColor{0.16F, 0.18F, 0.22F, 0.96F};
    glm::vec4 headerColor{0.10F, 0.12F, 0.16F, 1.0F};
    glm::vec4 headerAccentColor{0.28F, 0.38F, 0.52F, 0.95F};
    glm::vec4 borderColor{0.35F, 0.39F, 0.48F, 1.0F};
    glm::vec4 selectionColor{0.95F, 0.67F, 0.25F, 1.0F};
    glm::vec4 textColor{0.95F, 0.96F, 0.98F, 1.0F};
    glm::vec4 iconColor{0.95F, 0.96F, 0.98F, 1.0F};
    float borderThickness{1.5F};
    float headerHeight{24.0F};
    float cornerRadius{10.0F};
};

class NodeRenderer {
  private:
    unsigned int m_fillVao{0};
    unsigned int m_fillVbo{0};
    unsigned int m_lineVao{0};
    unsigned int m_lineVbo{0};
    unsigned int m_program{0};
    unsigned int m_textVao{0};
    unsigned int m_textVbo{0};

  public:
    using StyleResolver = std::function<NodeRenderStyle(Core::Ids::NodeTypeId)>;
    using TitleResolver = std::function<std::string(Core::Ids::NodeTypeId)>;
    using IconResolver = std::function<std::string(Core::Ids::NodeTypeId)>;

    bool init();
    void shutdown();

    void render(const std::vector<Scene::NodeRenderData>& nodes, const Camera::Camera2D& camera,
                const std::vector<Core::Ids::NodeId>& selectedNodeIds, const StyleResolver& styleResolver = {},
                const TitleResolver& titleResolver = {}, const IconResolver& iconResolver = {}) const;

    [[nodiscard]] static NodeRenderStyle defaultStyle(Core::Ids::NodeTypeId typeId);
};

} // namespace ScopeCanvas::Render::Renderers

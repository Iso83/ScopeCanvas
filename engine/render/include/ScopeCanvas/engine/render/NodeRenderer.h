#pragma once

#include <functional>
#include <glm/vec2.hpp>
#include <ScopeCanvas/engine/core/ids/CanvasIds.h>
#include <ScopeCanvas/engine/core/ids/TypeIds.h>
#include <ScopeCanvas/engine/render/NodeRenderStyle.h>
#include <ScopeCanvas/engine/render/scene/RenderScene.h>
#include <vector>

namespace ScopeCanvas::Engine::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Engine::Render {

class NodeRenderer {
  private:
    unsigned int m_fillVao{0};
    unsigned int m_fillVbo{0};
    unsigned int m_lineVao{0};
    unsigned int m_lineVbo{0};
    unsigned int m_program{0};

  public:
    using StyleResolver = std::function<NodeRenderStyle(Core::Ids::NodeTypeId)>;

    bool init();
    void shutdown();

    void render(const std::vector<Scene::NodeRenderData>& nodes, const Camera::Camera2D& camera,
                const std::vector<Core::Ids::NodeId>& selectedNodeIds, const StyleResolver& styleResolver = {}) const;
    void renderSelectionBorders(const std::vector<Scene::NodeRenderData>& nodes, const Camera::Camera2D& camera,
                                const std::vector<Core::Ids::NodeId>& selectedNodeIds,
                                const StyleResolver& styleResolver = {}) const;

    [[nodiscard]] virtual glm::vec2 nodeSize(const Scene::NodeRenderData& node) const;
    [[nodiscard]] static NodeRenderStyle defaultStyle(Core::Ids::NodeTypeId typeId);
};

} // namespace ScopeCanvas::Render

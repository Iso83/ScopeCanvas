#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/render/scene/RenderScene.h>
#include <vector>

namespace ScopeCanvas::Render::Camera {
class Camera2D;
}

namespace ScopeCanvas::Render::Renderers {
class NodeRenderer {
  public:
    bool init();
    void shutdown();

    void render(const std::vector<Scene::NodeRenderData>& nodes, const Camera::Camera2D& camera,
                Core::CanvasNodeId selectedNodeId) const;

  private:
    unsigned int m_vao{0};
    unsigned int m_vbo{0};
    unsigned int m_program{0};
};

} // namespace ScopeCanvas::Render::Renderers

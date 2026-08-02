#pragma once

#include <ScopeCanvas/engine/render/camera/Camera2D.h>
#include <ScopeCanvas/engine/routing/flow/FlowLayout.h>
#include <memory>

namespace ScopeCanvas::Engine::Render::Flow {

struct FlowGroupHeaderGeometry {
    glm::vec2 textPosition{};
    glm::vec2 togglePosition{};
    glm::vec2 toggleSize{};
};

struct FlowRenderOptions {
    bool showGrid{false};
    Core::Ids::NodeId selectedStep{};
};

class FlowRenderer {
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

public:
    FlowRenderer();
    ~FlowRenderer();
    FlowRenderer(const FlowRenderer&) = delete;
    FlowRenderer& operator=(const FlowRenderer&) = delete;

    bool init();
    void shutdown();
    void render(const Core::Flow::FlowDocument& document, const Routing::Flow::FlowLayoutResult& layout,
                const Camera::Camera2D& camera, const FlowRenderOptions& options) const;
};

[[nodiscard]] FlowGroupHeaderGeometry groupHeaderGeometry(const Routing::Flow::FlowGroupLayout& group,
                                                          float visibleCenterX, float labelWidth = 240.0F);

} // namespace ScopeCanvas::Engine::Render::Flow

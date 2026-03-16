#pragma once

#include <vector>

#include "Scene/RenderScene.h"

namespace ScopeCanvas::Render::Renderers
{
class NodeRenderer
{
public:
    [[nodiscard]] std::vector<Scene::NodeRenderData> collect(const Scene::RenderScene& scene) const;
};
} // namespace ScopeCanvas::Render::Renderers

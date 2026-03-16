#pragma once

#include <vector>

#include "Scene/RenderScene.h"

namespace ScopeCanvas::RenderGL::Renderers
{
class NodeRenderer
{
public:
    [[nodiscard]] std::vector<Scene::NodeRenderData> collect(const Scene::RenderScene& scene) const;
};
} // namespace ScopeCanvas::RenderGL::Renderers

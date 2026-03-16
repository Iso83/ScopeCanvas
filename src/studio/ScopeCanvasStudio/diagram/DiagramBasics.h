#pragma once

#include "ScopeCanvasEngineCore/Core/DiagramModel.h"
#include "ScopeCanvasRouting/Routing/EdgeRouter.h"

#include <vector>

namespace ScopeCanvas::Studio
{
struct GridSettings
{
    bool enabled{true};
    bool snapEnabled{true};
    float cellSize{32.0F};
};

struct ViewState
{
    float cameraX{0.0F};
    float cameraY{0.0F};
    float zoom{1.0F};
};

class DiagramBasics
{
public:
    DiagramBasics();

    Engine::Core::DiagramModel& model();
    const Engine::Core::DiagramModel& model() const;

    GridSettings& gridSettings();
    const GridSettings& gridSettings() const;

    std::vector<Engine::Core::CanvasNodeId>& nodeIds();
    const std::vector<Engine::Core::CanvasNodeId>& nodeIds() const;

    std::vector<Engine::Core::CanvasEdgeId>& edgeIds();
    const std::vector<Engine::Core::CanvasEdgeId>& edgeIds() const;

    Engine::Core::CanvasNodeId createNode(Engine::Core::NodeTypeId typeId, Engine::Core::Vec2 position);
    Engine::Core::CanvasEdgeId connect(Engine::Core::CanvasConnectorId a, Engine::Core::CanvasConnectorId b);
    void deleteNode(Engine::Core::CanvasNodeId nodeId);

    std::vector<Engine::Routing::EdgeRoute> routeAllEdges() const;

private:
    Engine::Core::DiagramModel m_model{};
    GridSettings m_grid{};
    std::vector<Engine::Core::CanvasNodeId> m_nodeIds{};
    std::vector<Engine::Core::CanvasEdgeId> m_edgeIds{};
};
} // namespace ScopeCanvas::Studio

#pragma once

#include <ScopeCanvas/demo/common/DiagramBasics.h>
#include <ScopeCanvas/engine/render/CanvasRenderer.h>
#include <ScopeCanvas/engine/render/camera/Camera2D.h>
#include <ScopeCanvas/engine/render/window/DrawContext.h>
#include <ScopeCanvas/widget/render/NodeInfo.h>

namespace ScopeCanvas::Demo::Common {
class DiagramDrawCtx : public Engine::Render::Window::DrawContext {
private:
    struct DrawFrameState {
        Engine::Render::Window::Viewport* view{};
        Engine::Render::Window::ViewportHandler* handler{};
        bool isActiveView{};
        float zoom{};
        Engine::Render::Camera::Camera2D camera{};
        glm::vec2 mouseWorld{};
    };

    //-------------------------------------------------------------------------
    // Document
    //-------------------------------------------------------------------------
    DiagramBasics m_basics{};

    //-------------------------------------------------------------------------
    // Renderers
    //-------------------------------------------------------------------------
    Engine::Render::CanvasRenderer m_renderer{};
    bool m_rendererInitialized{false};

    Widget::Render::NodeInfoRenderer m_nodeInfoRenderer{};
    bool m_nodeInfoRendererInitialized{false};

    //-------------------------------------------------------------------------
    // Interaction: Selection
    //-------------------------------------------------------------------------
    Engine::Core::Ids::EdgeId m_selectedEdge{};

    Engine::Core::Ids::ConnectorId m_hoveredConnector{};

    bool m_selectionRectActive{};
    glm::vec2 m_selectionRectStart{};
    glm::vec2 m_selectionRectEnd{};

    //-------------------------------------------------------------------------
    // Interaction: Drag
    //-------------------------------------------------------------------------
    glm::vec2 m_dragOffset{};
    glm::vec2 m_dragAnchorStartPosition{};
    Engine::Core::Ids::NodeId m_dragNode{};

    std::vector<glm::vec2> m_dragSelectionStartPositions{};
    std::vector<Engine::Core::Ids::NodeId> m_dragSelection{};

    //-------------------------------------------------------------------------
    // Interaction: Connector
    //-------------------------------------------------------------------------
    Engine::Core::Ids::ConnectorId m_pendingConnector{};

    bool m_reconnectingEdge{};
    bool m_reconnectingFromStart{};

    Engine::Core::Ids::ConnectorId m_reconnectFixedConnector{};
    Engine::Core::Ids::ConnectorId m_reconnectOriginalFrom{};
    Engine::Core::Ids::ConnectorId m_reconnectOriginalTo{};

    //-------------------------------------------------------------------------
    // State
    //-------------------------------------------------------------------------
    bool m_showGrid{true};
    bool m_needsRender{true};

public:
    ~DiagramDrawCtx();

    //-------------------------------------------------------------------------
    // Rendering
    //-------------------------------------------------------------------------
    void draw(Engine::Render::Window::Viewport* view) override;

    bool& showGrid() {
        return m_showGrid;
    }

    bool needsRender() override {
        return m_needsRender || m_pendingConnector.isValid() || m_dragNode.isValid() || m_selectionRectActive ||
               m_reconnectingEdge;
    }

    //-------------------------------------------------------------------------
    // Interaction: Selection
    //-------------------------------------------------------------------------
    [[nodiscard]] Engine::Core::Ids::EdgeId selectedEdge() const {
        return m_selectedEdge;
    }
    void clearSelectedEdge() {
        m_selectedEdge = {};
    }
    void deleteSelection();

    //-------------------------------------------------------------------------
    // Helpers
    //-------------------------------------------------------------------------
    DiagramBasics& document() {
        return m_basics;
    }
    Engine::Core::Ids::NodeId createNodeAtCenter(const Engine::Render::Camera::Camera2D& cam,
                                                 Engine::Core::Ids::NodeTypeId typeId);

private:
    //-------------------------------------------------------------------------
    // Rendering
    //-------------------------------------------------------------------------
    std::vector<Engine::Routing::EdgeRoute> buildRoutes() const;
    Engine::Render::CanvasRenderOptions buildRenderOptions(const DrawFrameState& frame,
                                                           const std::vector<Engine::Routing::EdgeRoute>& routes);

    void renderDiagram(const DrawFrameState& frame, const std::vector<Engine::Routing::EdgeRoute>& routes,
                       const Engine::Render::CanvasRenderOptions& options);

    //-------------------------------------------------------------------------
    // Interaction
    //-------------------------------------------------------------------------
    void handleActiveInteraction(const DrawFrameState& frame, const Engine::Render::CanvasRenderOptions& options);

    void processActiveCameraPan(const DrawFrameState& frame);
    void processActiveMousePressed(const DrawFrameState& frame, const Engine::Render::CanvasRenderOptions& options);
    void processActiveMouseDrag(const DrawFrameState& frame);
    void processActiveMouseReleased(const DrawFrameState& frame);
    void processActiveKeyboard(const DrawFrameState& frame);

    void beginConnectorInteraction();
    void beginNodeDrag(Engine::Core::Ids::NodeId nodeId, const glm::vec2& mouseWorld);
    void beginEdgeSelection(Engine::Core::Ids::EdgeId edgeId);
    void beginSelectionRect(const glm::vec2& mouseWorld);

    void finishConnectorInteraction(const DrawFrameState& frame);
    void clearDragState();
    void clearReconnectState();

    //-------------------------------------------------------------------------
    // State
    //-------------------------------------------------------------------------
    void markNeedsRender();
    DrawFrameState frameState(Engine::Render::Window::Viewport* view) const;

    //-------------------------------------------------------------------------
    // Helpers: Geometry
    //-------------------------------------------------------------------------
    glm::vec2 snapToGrid(glm::vec2 position) const;
    glm::vec2 connectorWorld(const Engine::Core::Node& node, std::size_t index) const;

    //-------------------------------------------------------------------------
    // Helpers: Picking
    //-------------------------------------------------------------------------
    Engine::Core::Ids::NodeId pickNode(const glm::vec2& world) const;
    Engine::Core::Ids::ConnectorId pickConnector(const float camZoom, const glm::vec2& world) const;

    //-------------------------------------------------------------------------
    // Helpers: Selection
    //-------------------------------------------------------------------------
    void applySelectionRect();
};
} // namespace ScopeCanvas::Demo::Common

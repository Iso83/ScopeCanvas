#pragma once

#include <ScopeCanvas/demo/DiagramBasics.h>
#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/render/CanvasRenderer.h>
#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/widget/render/NodeInfo.h>

namespace ScopeCanvas::Demo {
class DiagramDrawCtx : public Render::Window::DrawContext {
  private:
    struct DrawFrameState {
        Render::Window::Viewport* view{};
        Render::Window::ViewportHandler* handler{};
        bool isActiveView{};
        float zoom{};
        Render::Camera::Camera2D camera{};
        glm::vec2 mouseWorld{};
    };

    //-------------------------------------------------------------------------
    // Document
    //-------------------------------------------------------------------------
    DiagramBasics m_basics{};

    //-------------------------------------------------------------------------
    // Renderers
    //-------------------------------------------------------------------------
    Render::CanvasRenderer m_renderer{};
    bool m_rendererInitialized{false};

    Widget::Render::NodeInfoRenderer m_nodeInfoRenderer{};
    bool m_nodeInfoRendererInitialized{false};

    //-------------------------------------------------------------------------
    // Interaction: Selection
    //-------------------------------------------------------------------------
    Core::Ids::EdgeId m_selectedEdge{};
    
    Core::Ids::ConnectorId m_hoveredConnector{};

    bool m_selectionRectActive{};
    glm::vec2 m_selectionRectStart{};
    glm::vec2 m_selectionRectEnd{};

    //-------------------------------------------------------------------------
    // Interaction: Drag
    //-------------------------------------------------------------------------
    glm::vec2 m_dragOffset{};
    glm::vec2 m_dragAnchorStartPosition{};
    Core::Ids::NodeId m_dragNode{};

    std::vector<glm::vec2> m_dragSelectionStartPositions{};
    std::vector<Core::Ids::NodeId> m_dragSelection{};

    //-------------------------------------------------------------------------
    // Interaction: Connector
    //-------------------------------------------------------------------------
    Core::Ids::ConnectorId m_pendingConnector{};

    bool m_reconnectingEdge{};
    bool m_reconnectingFromStart{};

    Core::Ids::ConnectorId m_reconnectFixedConnector{};
    Core::Ids::ConnectorId m_reconnectOriginalFrom{};
    Core::Ids::ConnectorId m_reconnectOriginalTo{};

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
    void draw(Render::Window::Viewport* view);

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
    [[nodiscard]] Core::Ids::EdgeId selectedEdge() const {
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
    Core::Ids::NodeId createNodeAtCenter(const Render::Camera::Camera2D& cam, Core::Ids::NodeTypeId typeId);

  private:
    //-------------------------------------------------------------------------
    // Rendering
    //-------------------------------------------------------------------------
    std::vector<Routing::EdgeRoute> buildRoutes() const;
    Render::CanvasRenderOptions buildRenderOptions(const DrawFrameState& frame,
                                                   const std::vector<Routing::EdgeRoute>& routes);

    void renderDiagram(const DrawFrameState& frame, const std::vector<Routing::EdgeRoute>& routes,
                       const Render::CanvasRenderOptions& options);

    //-------------------------------------------------------------------------
    // Interaction
    //-------------------------------------------------------------------------
    void handleActiveInteraction(const DrawFrameState& frame, const Render::CanvasRenderOptions& options);

    void processActiveCameraPan(const DrawFrameState& frame);
    void processActiveMousePressed(const DrawFrameState& frame, const Render::CanvasRenderOptions& options);
    void processActiveMouseDrag(const DrawFrameState& frame);
    void processActiveMouseReleased(const DrawFrameState& frame);
    void processActiveKeyboard(const DrawFrameState& frame);

    void beginConnectorInteraction();
    void beginNodeDrag(Core::Ids::NodeId nodeId, const glm::vec2& mouseWorld);
    void beginEdgeSelection(Core::Ids::EdgeId edgeId);
    void beginSelectionRect(const glm::vec2& mouseWorld);

    void finishConnectorInteraction(const DrawFrameState& frame);
    void clearDragState();
    void clearReconnectState();

    //-------------------------------------------------------------------------
    // State
    //-------------------------------------------------------------------------
    void markNeedsRender();
    DrawFrameState frameState(Render::Window::Viewport* view) const;

    //-------------------------------------------------------------------------
    // Helpers: Geometry
    //-------------------------------------------------------------------------
    glm::vec2 snapToGrid(glm::vec2 position) const;
    glm::vec2 connectorWorld(const Core::Node& node, std::size_t index) const;

    //-------------------------------------------------------------------------
    // Helpers: Picking
    //-------------------------------------------------------------------------
    Core::Ids::NodeId pickNode(const glm::vec2& world) const;
    Core::Ids::ConnectorId pickConnector(const float camZoom, const glm::vec2& world) const;

    //-------------------------------------------------------------------------
    // Helpers: Selection
    //-------------------------------------------------------------------------
    void applySelectionRect();
};
} // namespace ScopeCanvas::Demo
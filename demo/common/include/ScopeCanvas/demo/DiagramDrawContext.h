#pragma once

#include <ScopeCanvas/demo/DiagramBasics.h>
#include <ScopeCanvas/render/camera/Camera2D.h>
#include <ScopeCanvas/render/CanvasRenderer.h>
#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/widget/NodeInfo.h>

namespace ScopeCanvas::Demo {
class DiagramDrawCtx : public Render::Window::DrawContext {
  private:
    DiagramBasics m_basics{};

    Render::CanvasRenderer m_renderer{};
    Widget::NodeInfoRenderer m_nodeInfoRenderer{};
    bool m_rendererInitialized{false};
    bool m_nodeInfoRendererInitialized{false};

    Core::Ids::NodeId m_dragNode{};
    glm::vec2 m_dragOffset{};
    glm::vec2 m_dragAnchorStartPosition{};
    std::vector<Core::Ids::NodeId> m_dragSelection{};
    std::vector<glm::vec2> m_dragSelectionStartPositions{};

    Core::Ids::ConnectorId m_pendingConnector{};
    bool m_reconnectingEdge{false};
    bool m_reconnectingFromStart{false};
    Core::Ids::ConnectorId m_reconnectFixedConnector{};
    Core::Ids::ConnectorId m_reconnectOriginalFrom{};
    Core::Ids::ConnectorId m_reconnectOriginalTo{};

    Core::Ids::EdgeId m_selectedEdge{};

    Core::Ids::ConnectorId m_hoveredConnector{};
    bool m_selectionRectActive{false};
    glm::vec2 m_selectionRectStart{};
    glm::vec2 m_selectionRectEnd{};

    bool m_showGrid{true};
    bool m_showDebug{false};

  public:
    ~DiagramDrawCtx();

    inline DiagramBasics& Doc() {
        return m_basics;
    }

    void draw(Render::Window::Viewport* view);

    inline bool needsRender() {
        return true;
    }

    [[nodiscard]] Core::Ids::EdgeId selectedEdge() const {
        return m_selectedEdge;
    }
    void clearSelectedEdge() {
        m_selectedEdge = {};
    }

    void deleteSelection();

    Core::Ids::NodeId createNodeAtCenter(const Render::Camera::Camera2D& cam, Core::Ids::NodeTypeId typeId);

    bool& showGrid() {
        return m_showGrid;
    }

    bool& showDebug() {
        return m_showDebug;
    }

  private:
    glm::vec2 snapToGrid(glm::vec2 position) const;
    glm::vec2 connectorWorld(const Core::Node& node, std::size_t index) const;

    Core::Ids::NodeId pickNode(const glm::vec2& world) const;
    Core::Ids::ConnectorId pickConnector(const float camZoom, const glm::vec2& world) const;

    void applySelectionRect();
};
} // namespace ScopeCanvas::Demo
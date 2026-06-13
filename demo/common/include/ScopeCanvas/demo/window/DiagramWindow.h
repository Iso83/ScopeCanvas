#pragma once

#include <ScopeCanvas/demo/diagram/DiagramBasics.h>
#include <ScopeCanvas/render/CanvasRenderer.h>
#include <ScopeCanvas/render/camera/Camera2D.h>
#include <string>
#include <vector>

namespace ScopeCanvas::Demo {

struct DiagramInput {
    float mouseX{0.0F};
    float mouseY{0.0F};
    bool hovered{true};
    bool leftDown{false};
    bool leftPressed{false};
    bool leftReleased{false};
    bool middleDown{false};
    glm::vec2 mouseDelta{0.0F, 0.0F};
    float scrollDelta{0.0F};
    bool deletePressed{false};
};

class DiagramWindow {
  private:
    std::string m_title{};
    DiagramBasics* m_basics{};

    Render::CanvasRenderer m_renderer{};
    bool m_rendererInitialized{false};

    Render::Camera::Camera2D m_camera{};

    unsigned int m_framebuffer{0};
    unsigned int m_colorTexture{0};
    unsigned int m_depthStencilRenderbuffer{0};
    int m_renderWidth{0};
    int m_renderHeight{0};

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
    DiagramWindow(std::string title, DiagramBasics* basics);
    ~DiagramWindow();

    void draw(int width, int height, const DiagramInput& input);
    unsigned int drawToTexture(int width, int height, const DiagramInput& input);

    [[nodiscard]] Core::Ids::EdgeId selectedEdge() const {
        return m_selectedEdge;
    }
    void clearSelectedEdge() {
        m_selectedEdge = {};
    }
    void deleteSelection();
    Core::Ids::NodeId createNodeAtCenter(Core::Ids::NodeTypeId typeId);

    bool& showGrid() {
        return m_showGrid;
    }
    bool& showDebug() {
        return m_showDebug;
    }
    Render::Camera::Camera2D& camera() {
        return m_camera;
    }

  private:
    void renderContent(int width, int height, const DiagramInput& input);
    void ensureRenderTarget(int width, int height);
    void releaseRenderTarget();
    glm::vec2 snapToGrid(glm::vec2 position) const;
    glm::vec2 screenToWorld(float sx, float sy, float w, float h) const;
    glm::vec2 connectorWorld(const Core::Node& node, std::size_t index) const;

    Core::Ids::NodeId pickNode(const glm::vec2& world) const;
    Core::Ids::ConnectorId pickConnector(const glm::vec2& world) const;

    void applySelectionRect();
};

} // namespace ScopeCanvas::Demo

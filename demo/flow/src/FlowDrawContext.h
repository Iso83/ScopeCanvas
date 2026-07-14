#pragma once
#include <ScopeCanvas/render/window/DrawContext.h>
#include <ScopeCanvas/core/flow/FlowDocument.h>
#include <ScopeCanvas/render/NodeRenderer.h>
#include <ScopeCanvas/routing/flow/FlowLayout.h>
#include <ScopeCanvas/render/flow/FlowRenderer.h>

class FlowDrawContext : public ScopeCanvas::Render::Window::DrawContext {
  private:
    ScopeCanvas::Core::Flow::FlowDocument m_document{};
    ScopeCanvas::Routing::Flow::FlowLayout m_layoutEngine{};
    ScopeCanvas::Routing::Flow::FlowLayoutResult m_layout{};
    ScopeCanvas::Render::Flow::FlowRenderer m_renderer{};
    ScopeCanvas::Render::NodeRenderer m_scrollbarRenderer{};
    ScopeCanvas::Core::Ids::NodeId m_selected{};
    bool m_scrollDragging{false};
    bool m_scrollbarInitialized{false};
    bool m_needsRender{true};

    struct ScrollbarGeometry {
        glm::vec2 trackPosition{};
        glm::vec2 trackSize{};
        glm::vec2 thumbPosition{};
        glm::vec2 thumbSize{};
    };

  public:
    FlowDrawContext();
    ~FlowDrawContext();
    void draw(ScopeCanvas::Render::Window::Viewport* view) override;
    bool needsRender() override { return m_needsRender; }
    void clampViewToContent(ScopeCanvas::Render::Window::Viewport* view) const;

  private:
    ScopeCanvas::Core::Ids::NodeId pickStep(glm::vec2 world) const;
    bool pickCollapseToggle(glm::vec2 world, ScopeCanvas::Core::Ids::NodeId& stepId) const;
    bool pickGroupToggle(ScopeCanvas::Render::Window::Viewport* view, glm::vec2 world, ScopeCanvas::Core::Flow::FlowGroupId& groupId) const;
    bool scrollbarHit(ScopeCanvas::Render::Window::Viewport* view, glm::vec2 world) const;
    bool scrollbarGeometry(ScopeCanvas::Render::Window::Viewport* view, ScrollbarGeometry& geometry) const;
    void applyScrollbarDrag(ScopeCanvas::Render::Window::Viewport* view, glm::vec2 world) const;
    void renderScrollbar(ScopeCanvas::Render::Window::Viewport* view);
    void rebuildLayout();
};

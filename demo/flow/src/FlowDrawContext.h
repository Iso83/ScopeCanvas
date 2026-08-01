#pragma once
#include <ScopeCanvas/engine/core/flow/FlowDocument.h>
#include <ScopeCanvas/engine/render/NodeRenderer.h>
#include <ScopeCanvas/engine/render/flow/FlowRenderer.h>
#include <ScopeCanvas/engine/render/window/DrawContext.h>
#include <ScopeCanvas/engine/routing/flow/FlowLayout.h>

namespace ScopeCanvas::Demo::Flow {
class FlowDrawContext : public Engine::Render::Window::DrawContext {
private:
    Engine::Core::Flow::FlowDocument m_document{};
    Engine::Routing::Flow::FlowLayout m_layoutEngine{};
    Engine::Routing::Flow::FlowLayoutResult m_layout{};
    Engine::Render::Flow::FlowRenderer m_renderer{};
    Engine::Render::NodeRenderer m_scrollbarRenderer{};
    Engine::Core::Ids::NodeId m_selected{};
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
    void draw(Engine::Render::Window::Viewport* view) override;
    bool needsRender() override {
        return m_needsRender;
    }
    void clampViewToContent(Engine::Render::Window::Viewport* view) const;

private:
    Engine::Core::Ids::NodeId pickStep(glm::vec2 world) const;
    bool pickCollapseToggle(glm::vec2 world, Engine::Core::Ids::NodeId& stepId) const;
    bool pickGroupToggle(Engine::Render::Window::Viewport* view, glm::vec2 world,
                         Engine::Core::Flow::FlowGroupId& groupId) const;
    bool scrollbarHit(Engine::Render::Window::Viewport* view, glm::vec2 world) const;
    bool scrollbarGeometry(Engine::Render::Window::Viewport* view, ScrollbarGeometry& geometry) const;
    void applyScrollbarDrag(Engine::Render::Window::Viewport* view, glm::vec2 world) const;
    void renderScrollbar(Engine::Render::Window::Viewport* view);
    void rebuildLayout();
};
} // namespace ScopeCanvas::Demo::Flow

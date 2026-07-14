#include "FlowDrawContext.h"
#include <ScopeCanvas/render/scene/RenderScene.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>
#include <algorithm>
#include <cstdint>
#include <vector>

using namespace ScopeCanvas::Core::Ids;
using namespace ScopeCanvas::Core::Flow;
using namespace ScopeCanvas::Render;
using namespace ScopeCanvas::Render::Scene;
using namespace ScopeCanvas::Render::Flow;
using namespace ScopeCanvas::Routing::Flow;
using namespace ScopeCanvas::Render::Window;

namespace {
enum class DemoStepKind { Source = 1, Filter = 2, Join = 3, Transform = 4, Aggregate = 5, Export = 6 };

NodeTypeId typeId(DemoStepKind kind) {
    return NodeTypeId{static_cast<std::uint32_t>(kind)};
}
}

FlowDrawContext::FlowDrawContext() {
    FlowGroup& customers = m_document.createGroup("Customer Import", "Build a customer overview from MySQL data");
    FlowRow& customerRow = m_document.createRow(customers, "Customer flow");
    m_document.insertStep(customerRow, 0, typeId(DemoStepKind::Source), "MySQL Customers", "Customers table",
                          "2450 rows  12 ms");
    const NodeId filterActive = m_document.insertStep(customerRow, 1, typeId(DemoStepKind::Filter),
                                                      "SELECT active customers", "Active customers", "1812 rows  3 ms")
                                    .id;
    const NodeId joinOrders = m_document.insertStep(customerRow, 2, typeId(DemoStepKind::Join), "JOIN customer orders",
                                                    "Customer orders", "1812 rows  18 ms")
                                  .id;
    m_document.insertStep(customerRow, 3, typeId(DemoStepKind::Export), "Export customer overview", "CSV export",
                          "1812 rows  9 ms");

    NodeId filterDetail{};
    if (FlowStep* select = m_document.getStep(filterActive); select != nullptr) {
        filterDetail = m_document.addChildStep(*select, typeId(DemoStepKind::Filter), "Filter active customers",
                                               "Active = 1", "1812 rows  2 ms")
                           .id;
    }
    if (FlowStep* filter = m_document.getStep(filterDetail); filter != nullptr) {
        m_document.addChildStep(*filter, typeId(DemoStepKind::Transform), "Replace missing country",
                                "Country = Unknown", "0 rows  0 ms");
    }
    if (FlowStep* join = m_document.getStep(joinOrders); join != nullptr) {
        m_document.addChildStep(*join, typeId(DemoStepKind::Join), "Customer orders", "Orders table", "0 rows  0 ms");
    }

    FlowGroup& orders = m_document.createGroup("Order Summary", "Aggregate completed orders for export");
    FlowRow& orderRow = m_document.createRow(orders, "Order flow");
    m_document.insertStep(orderRow, 0, typeId(DemoStepKind::Source), "MySQL Orders", "Orders table", "3860 rows  15 ms");
    m_document.insertStep(orderRow, 1, typeId(DemoStepKind::Filter), "Filter completed orders", "Status = Completed",
                          "2440 rows  4 ms");
    m_document.insertStep(orderRow, 2, typeId(DemoStepKind::Aggregate), "Group by customer", "Aggregate totals",
                          "640 rows  7 ms");
    m_document.insertStep(orderRow, 3, typeId(DemoStepKind::Transform), "Calculate total", "Order total",
                          "2440 rows  5 ms");
    m_document.insertStep(orderRow, 4, typeId(DemoStepKind::Export), "Export order totals", "JSON export",
                          "640 rows  6 ms");
    rebuildLayout();
}

FlowDrawContext::~FlowDrawContext() {
    m_renderer.shutdown();
    if (m_scrollbarInitialized)
        m_scrollbarRenderer.shutdown();
}

void FlowDrawContext::draw(Viewport* view) {
    if (view == nullptr)
        return;
    const bool rendererInitialized = m_renderer.init();
    if (!m_scrollbarInitialized)
        m_scrollbarInitialized = m_scrollbarRenderer.init();

    const auto* handler = view->handler();
    const bool active = handler != nullptr && handler->activeViewport() == view;
    if (active && handler->mouseState(SC_MOUSE_BUTTON_MIDDLE).down) {
        const glm::vec2 mouseDelta = handler->mouseDeltaPosition();
        view->moveView({-mouseDelta.x / view->viewZoom(), mouseDelta.y / view->viewZoom()});
        clampViewToContent(view);
        m_needsRender = true;
    }
    if (active && handler->mouseState(SC_MOUSE_BUTTON_LEFT).pressed()) {
        const glm::vec2 world = view->screenToWorld(handler->mousePosition().x, handler->mousePosition().y);
        NodeId toggleStep{};
        FlowGroupId toggleGroup{};
        if (scrollbarHit(view, world)) {
            m_scrollDragging = true;
            applyScrollbarDrag(view, world);
        } else if (pickGroupToggle(view, world, toggleGroup)) {
            m_document.toggleGroupCollapsed(toggleGroup);
            rebuildLayout();
        } else if (pickCollapseToggle(world, toggleStep)) {
            m_document.toggleCollapsed(toggleStep);
            m_selected = toggleStep;
            rebuildLayout();
        } else {
            m_selected = pickStep(world);
        }
        m_needsRender = true;
    }
    if (active && handler->mouseState(SC_MOUSE_BUTTON_LEFT).down && m_scrollDragging) {
        const glm::vec2 world = view->screenToWorld(handler->mousePosition().x, handler->mousePosition().y);
        applyScrollbarDrag(view, world);
        m_needsRender = true;
    }
    if (active && handler->mouseState(SC_MOUSE_BUTTON_LEFT).released())
        m_scrollDragging = false;

    clampViewToContent(view);
    if (rendererInitialized) {
        m_renderer.render(m_document, m_layout, view->camera(), {false, m_selected});
        renderScrollbar(view);
    }
    m_needsRender = false;
}

NodeId FlowDrawContext::pickStep(glm::vec2 world) const {
    for (const FlowStepLayout& step : m_layout.steps)
        if (world.x >= step.position.x && world.x <= step.position.x + step.size.x && world.y >= step.position.y &&
            world.y <= step.position.y + step.size.y)
            return step.stepId;
    return {};
}

bool FlowDrawContext::pickCollapseToggle(glm::vec2 world, NodeId& stepId) const {
    for (const FlowStepLayout& step : m_layout.steps) {
        if (!step.hasChildren)
            continue;
        const glm::vec2 min = step.position + glm::vec2{10.0F, step.size.y - 28.0F};
        const glm::vec2 max = min + glm::vec2{24.0F, 24.0F};
        if (world.x >= min.x && world.x <= max.x && world.y >= min.y && world.y <= max.y) {
            stepId = step.stepId;
            return true;
        }
    }
    return false;
}

bool FlowDrawContext::pickGroupToggle(Viewport* view, glm::vec2 world, FlowGroupId& groupId) const {
    if (view == nullptr)
        return false;
    const float visibleCenterX = view->camera().position().x;
    for (const FlowGroupLayout& group : m_layout.groups) {
        const FlowGroupHeaderGeometry header = groupHeaderGeometry(group, visibleCenterX);
        const glm::vec2 min = header.togglePosition;
        const glm::vec2 max = min + header.toggleSize;
        if (world.x >= min.x && world.x <= max.x && world.y >= min.y && world.y <= max.y) {
            groupId = group.groupId;
            return true;
        }
    }
    return false;
}

bool FlowDrawContext::scrollbarHit(Viewport* view, glm::vec2 world) const {
    ScrollbarGeometry geometry{};
    if (!scrollbarGeometry(view, geometry))
        return false;
    return world.x >= geometry.trackPosition.x && world.x <= geometry.trackPosition.x + geometry.trackSize.x &&
           world.y >= geometry.trackPosition.y && world.y <= geometry.trackPosition.y + geometry.trackSize.y;
}

bool FlowDrawContext::scrollbarGeometry(Viewport* view, ScrollbarGeometry& geometry) const {
    if (view == nullptr || m_layout.rows.empty())
        return false;

    float contentLeft = m_layout.rows.front().railStart.x;
    float contentRight = m_layout.rows.front().railEnd.x;
    for (const FlowRowLayout& row : m_layout.rows) {
        contentLeft = std::min(contentLeft, row.railStart.x);
        contentRight = std::max(contentRight, row.railEnd.x);
    }

    const auto& camera = view->camera();
    const float viewportWidth = static_cast<float>(camera.getViewportWidth()) / camera.zoom();
    const float viewportHeight = static_cast<float>(camera.getViewportHeight()) / camera.zoom();
    const float visibleLeft = camera.position().x - viewportWidth * 0.5F;
    const float visibleBottom = camera.position().y - viewportHeight * 0.5F;
    const float trackPadding = 56.0F / camera.zoom();
    const float trackWidth = std::max(80.0F, viewportWidth - trackPadding * 2.0F);
    const float contentWidth = std::max(contentRight - contentLeft, 1.0F);
    const float minThumbWidth = std::min(48.0F / camera.zoom(), trackWidth);
    const float thumbWidth = std::clamp((viewportWidth / contentWidth) * trackWidth, minThumbWidth, trackWidth);
    const float scrollableContent = std::max(contentWidth - viewportWidth, 1.0F);
    const float t = std::clamp((visibleLeft - contentLeft) / scrollableContent, 0.0F, 1.0F);
    geometry.trackPosition = {visibleLeft + trackPadding, visibleBottom + 28.0F / camera.zoom()};
    geometry.trackSize = {trackWidth, 16.0F / camera.zoom()};
    geometry.thumbSize = {thumbWidth, geometry.trackSize.y};
    geometry.thumbPosition = {geometry.trackPosition.x + t * (trackWidth - thumbWidth), geometry.trackPosition.y};
    return true;
}

void FlowDrawContext::applyScrollbarDrag(Viewport* view, glm::vec2 world) const {
    ScrollbarGeometry geometry{};
    if (!scrollbarGeometry(view, geometry))
        return;

    float contentLeft = m_layout.rows.front().railStart.x;
    float contentRight = m_layout.rows.front().railEnd.x;
    for (const FlowRowLayout& row : m_layout.rows) {
        contentLeft = std::min(contentLeft, row.railStart.x);
        contentRight = std::max(contentRight, row.railEnd.x);
    }

    const auto& camera = view->camera();
    const float viewportWidth = static_cast<float>(camera.getViewportWidth()) / camera.zoom();
    const float contentWidth = std::max(contentRight - contentLeft, 1.0F);
    const float t = std::clamp((world.x - geometry.trackPosition.x - geometry.thumbSize.x * 0.5F) /
                                   std::max(geometry.trackSize.x - geometry.thumbSize.x, 1.0F),
                               0.0F, 1.0F);
    const float targetVisibleLeft = contentLeft + t * std::max(contentWidth - viewportWidth, 1.0F);
    view->setViewPosition({targetVisibleLeft + viewportWidth * 0.5F, camera.position().y});
    clampViewToContent(view);
}

void FlowDrawContext::renderScrollbar(Viewport* view) {
    ScrollbarGeometry geometry{};
    if (!m_scrollbarInitialized || !scrollbarGeometry(view, geometry))
        return;

    const std::vector<NodeRenderData> parts{
        {NodeId{930001}, NodeTypeId{91}, geometry.trackPosition, geometry.trackSize, 0},
        {NodeId{930002}, NodeTypeId{92}, geometry.thumbPosition, geometry.thumbSize, 0},
    };
    m_scrollbarRenderer.render(parts, view->camera(), {}, [](NodeTypeId typeId) {
        NodeRenderStyle style{};
        style.bodyColor = typeId.value() == 92U ? glm::vec4{0.62F, 0.72F, 0.86F, 0.96F}
                                                : glm::vec4{0.18F, 0.22F, 0.28F, 0.78F};
        style.borderColor = {0.34F, 0.42F, 0.52F, 0.85F};
        style.cornerRadius = 3.0F;
        style.borderThickness = 1.0F;
        return style;
    });
}

void FlowDrawContext::clampViewToContent(Viewport* view) const {
    if (view == nullptr || m_layout.rows.empty())
        return;

    float contentLeft = m_layout.rows.front().railStart.x;
    float contentRight = m_layout.rows.front().railEnd.x;
    float contentBottom = m_layout.rows.front().bottom;
    float contentTop = m_layout.rows.front().top;

    for (const FlowRowLayout& row : m_layout.rows) {
        contentLeft = std::min(contentLeft, row.railStart.x);
        contentRight = std::max(contentRight, row.railEnd.x);
        contentBottom = std::min(contentBottom, row.bottom);
        contentTop = std::max(contentTop, row.top);
    }
    for (const FlowGroupLayout& group : m_layout.groups) {
        contentLeft = std::min(contentLeft, group.boundsPosition.x);
        contentRight = std::max(contentRight, group.boundsPosition.x + group.boundsSize.x);
        contentBottom = std::min(contentBottom, group.boundsPosition.y);
        contentTop = std::max(contentTop, group.boundsPosition.y + group.boundsSize.y);
    }

    const auto& camera = view->camera();
    const float halfWidth = static_cast<float>(camera.getViewportWidth()) * 0.5F / camera.zoom();
    const float halfHeight = static_cast<float>(camera.getViewportHeight()) * 0.5F / camera.zoom();
    const float padding = 96.0F;

    const float minX = contentLeft + halfWidth - padding;
    const float maxX = contentRight - halfWidth + padding;
    const float x = minX > maxX ? (contentLeft + contentRight) * 0.5F : std::clamp(camera.position().x, minX, maxX);

    const float minY = contentBottom + halfHeight - padding;
    const float maxY = contentTop - halfHeight + padding;
    const float y = minY > maxY ? (contentBottom + contentTop) * 0.5F : std::clamp(camera.position().y, minY, maxY);

    view->setViewPosition({x, y});
}

void FlowDrawContext::rebuildLayout() {
    m_layout = m_layoutEngine.build(m_document);
    m_needsRender = true;
}

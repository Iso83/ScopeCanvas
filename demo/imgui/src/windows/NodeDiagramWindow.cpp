#include "windows/NodeDiagramWindow.h"

#include <ScopeCanvas/widget/theme/NodeVisualRegistry.h>
#include <algorithm>
#include <cmath>
#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <imgui.h>

namespace ScopeCanvas::Studio {
namespace {
bool pointInNode(const glm::vec2& point, const Core::Node& node) {
    return point.x >= node.position.x && point.x <= node.position.x + node.size.x && point.y >= node.position.y &&
           point.y <= node.position.y + node.size.y;
}

float distanceToSegmentSquared(const glm::vec2& point, const glm::vec2& a, const glm::vec2& b) {
    const glm::vec2 ab = b - a;
    const float len2 = glm::dot(ab, ab);
    if (len2 <= 0.0001F) {
        const glm::vec2 d = point - a;
        return glm::dot(d, d);
    }
    const float t = std::clamp(glm::dot(point - a, ab) / len2, 0.0F, 1.0F);
    const glm::vec2 proj = a + ab * t;
    const glm::vec2 d = point - proj;
    return glm::dot(d, d);
}

bool canConnect(const Core::GraphDocument& model, Core::CanvasConnectorId a, Core::CanvasConnectorId b) {
    if (!a.isValid() || !b.isValid() || a == b) {
        return false;
    }
    const auto* connectorA = model.getConnector(a);
    const auto* connectorB = model.getConnector(b);
    if (connectorA == nullptr || connectorB == nullptr || connectorA->nodeId == connectorB->nodeId) {
        return false;
    }
    for (const auto edgeId : connectorA->edges) {
        const auto* edge = model.getEdge(edgeId);
        if (edge != nullptr && ((edge->fromConnector == a && edge->toConnector == b) ||
                                (edge->fromConnector == b && edge->toConnector == a))) {
            return false;
        }
    }
    return true;
}

Render::Renderers::NodeRenderStyle toRenderStyle(const Render::Theme::NodeVisual& visual) {
    auto rgba = [](const Render::Theme::ColorRgba8& color) {
        return glm::vec4(static_cast<float>(color.r) / 255.0F, static_cast<float>(color.g) / 255.0F,
                         static_cast<float>(color.b) / 255.0F, static_cast<float>(color.a) / 255.0F);
    };

    Render::Renderers::NodeRenderStyle style{};
    style.bodyColor = rgba(visual.bodyColor);
    style.headerColor = rgba(visual.titleBarColor);
    style.headerAccentColor = rgba(visual.titleBarAccentColor);
    style.borderColor = rgba(visual.borderColor);
    style.selectionColor = rgba(visual.selectionColor);
    style.textColor = rgba(visual.titleTextColor);
    style.iconColor = rgba(visual.iconColor);
    style.borderThickness = visual.borderThickness;
    style.headerHeight = visual.titleBarHeight;
    style.cornerRadius = visual.cornerRadius;
    return style;
}
} // namespace

NodeDiagramWindow::NodeDiagramWindow(GLFWwindow* window, DiagramBasics* basics, ViewState* viewState, std::string title)
    : m_window(window), m_basics(basics), m_viewState(viewState), m_title(std::move(title)) {}

NodeDiagramWindow::~NodeDiagramWindow() {
    releaseRenderTarget();
    if (m_rendererInitialized) {
        m_renderer.shutdown();
    }
}

Core::CanvasEdgeId NodeDiagramWindow::selectedEdge() const {
    return m_selectedEdge;
}

void NodeDiagramWindow::clearSelectedEdge() {
    m_selectedEdge = {};
}

void NodeDiagramWindow::ensureRenderTarget(int width, int height) {
    width = std::max(width, 1);
    height = std::max(height, 1);
    if (m_framebuffer != 0 && m_renderWidth == width && m_renderHeight == height) {
        return;
    }

    releaseRenderTarget();
    m_renderWidth = width;
    m_renderHeight = height;

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_renderWidth, m_renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

    glGenRenderbuffers(1, &m_depthStencilRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_renderWidth, m_renderHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilRenderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void NodeDiagramWindow::releaseRenderTarget() {
    if (m_depthStencilRenderbuffer != 0) {
        glDeleteRenderbuffers(1, &m_depthStencilRenderbuffer);
    }
    if (m_colorTexture != 0) {
        glDeleteTextures(1, &m_colorTexture);
    }
    if (m_framebuffer != 0) {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
    m_depthStencilRenderbuffer = 0;
    m_colorTexture = 0;
    m_framebuffer = 0;
}

glm::vec2 NodeDiagramWindow::screenToWorld(float sx, float sy, float w, float h) const {
    const float nx = (sx / w) * 2.0F - 1.0F;
    const float ny = 1.0F - (sy / h) * 2.0F;
    const glm::mat4 inv = m_camera.invViewProjection();
    const glm::vec4 v = inv * glm::vec4(nx, ny, 0.0F, 1.0F);
    return {v.x / v.w, v.y / v.w};
}

glm::vec2 NodeDiagramWindow::connectorWorld(const Core::Node& node, std::size_t index) const {
    const bool output = (index % 2U) == 1U;
    const std::size_t sideIndex = index / 2U;
    const std::size_t sideCount = output ? node.connectors.size() / 2U : (node.connectors.size() + 1U) / 2U;
    constexpr float headerHeight = 24.0F;
    constexpr float verticalInset = 12.0F;
    const float bodyMinY = node.position.y + verticalInset;
    const float bodyMaxY = std::max(bodyMinY + 1.0F, node.position.y + node.size.y - headerHeight - verticalInset);
    const float bodyHeight = std::max(bodyMaxY - bodyMinY, 1.0F);
    const float step = bodyHeight / static_cast<float>(sideCount + 1U);
    const float y = bodyMinY + step * static_cast<float>(sideIndex + 1U);
    return {output ? node.position.x + node.size.x : node.position.x, y};
}

Core::CanvasNodeId NodeDiagramWindow::pickNode(const glm::vec2& world) const {
    for (auto it = m_basics->nodeIds().rbegin(); it != m_basics->nodeIds().rend(); ++it) {
        const Core::Node* node = m_basics->model().getNode(*it);
        if (node != nullptr && pointInNode(world, *node)) {
            return *it;
        }
    }
    return {};
}

Core::CanvasConnectorId NodeDiagramWindow::pickConnector(const glm::vec2& world) const {
    const float pickRadiusSquared = 100.0F / (m_viewState->zoom * m_viewState->zoom);
    for (auto it = m_basics->nodeIds().rbegin(); it != m_basics->nodeIds().rend(); ++it) {
        const Core::Node* node = m_basics->model().getNode(*it);
        if (node == nullptr) {
            continue;
        }
        for (std::size_t i = 0; i < node->connectors.size(); ++i) {
            const glm::vec2 delta = world - connectorWorld(*node, i);
            if (glm::dot(delta, delta) <= pickRadiusSquared) {
                return node->connectors[i];
            }
        }
    }
    return {};
}

void NodeDiagramWindow::applySelectionRect() {
    const glm::vec2 rectMin(std::min(m_selectionRectStart.x, m_selectionRectEnd.x),
                            std::min(m_selectionRectStart.y, m_selectionRectEnd.y));
    const glm::vec2 rectMax(std::max(m_selectionRectStart.x, m_selectionRectEnd.x),
                            std::max(m_selectionRectStart.y, m_selectionRectEnd.y));

    std::vector<Core::CanvasNodeId> selection;
    for (Core::CanvasNodeId nodeId : m_basics->nodeIds()) {
        const Core::Node* node = m_basics->model().getNode(nodeId);
        if (node == nullptr) {
            continue;
        }
        const glm::vec2 nodeMin = node->position;
        const glm::vec2 nodeMax = node->position + node->size;
        const bool overlapX = nodeMax.x >= rectMin.x && nodeMin.x <= rectMax.x;
        const bool overlapY = nodeMax.y >= rectMin.y && nodeMin.y <= rectMax.y;
        if (overlapX && overlapY) {
            selection.push_back(nodeId);
        }
    }
    m_basics->setSelection(selection);
}

glm::vec2 NodeDiagramWindow::snapToGrid(glm::vec2 position) const {
    if (!m_basics->gridSettings().snapEnabled) {
        return position;
    }
    const float s = m_basics->gridSettings().cellSize;
    position.x = std::round(position.x / s) * s;
    position.y = std::round(position.y / s) * s;
    return position;
}

void NodeDiagramWindow::draw() {
    if (m_basics == nullptr || m_viewState == nullptr) {
        return;
    }
    if (!m_rendererInitialized) {
        m_rendererInitialized = m_renderer.init();
    }

    ImGui::Begin(m_title.c_str());
    ImGui::Checkbox("Grid", &m_showGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Debug", &m_showDebug);

    const ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    ensureRenderTarget(static_cast<int>(canvasSize.x), static_cast<int>(canvasSize.y));

    m_camera.setViewportSize(std::max(1, static_cast<int>(canvasSize.x)), std::max(1, static_cast<int>(canvasSize.y)));
    m_camera.setPosition({m_viewState->cameraX, m_viewState->cameraY});
    m_camera.setZoom(m_viewState->zoom);

    GLint oldFb = 0;
    GLint oldVp[4]{};
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
    glGetIntegerv(GL_VIEWPORT, oldVp);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_renderWidth, m_renderHeight);

    const ImVec2 mouse = ImGui::GetIO().MousePos;
    const glm::vec2 mouseWorld =
        screenToWorld(mouse.x - canvasPos.x, mouse.y - canvasPos.y, std::max(canvasSize.x, 1.0F), std::max(canvasSize.y, 1.0F));

    const std::vector<Routing::EdgeRoute> routes = m_basics->routeAllEdges();

    Render::CanvasRenderOptions options{};
    options.showGrid = m_showGrid && m_basics->gridSettings().enabled;
    options.showDebug = m_showDebug;
    options.gridSize = m_basics->gridSettings().cellSize;
    options.selectedNodeIds = m_basics->selectedNodeIds();
    options.selectedEdgeId = m_selectedEdge;
    options.selectionRectActive = m_selectionRectActive;
    options.selectionRectStart = m_selectionRectStart;
    options.selectionRectEnd = m_selectionRectEnd;
    options.hoveredConnectorId = m_hoveredConnector;
    options.activeConnectorId = m_pendingConnector;
    options.previewEdgeActive = m_pendingConnector.isValid();
    options.previewEdgeEnd = mouseWorld;
    if (m_pendingConnector.isValid()) {
        const Core::Connector* connector = m_basics->model().getConnector(m_pendingConnector);
        const Core::Node* node = connector == nullptr ? nullptr : m_basics->model().getNode(connector->nodeId);
        if (node != nullptr) {
            for (std::size_t i = 0; i < node->connectors.size(); ++i) {
                if (node->connectors[i] == m_pendingConnector) {
                    options.previewEdgeStart = connectorWorld(*node, i);
                    options.previewEdgeStartNormal = (i % 2U) == 1U ? glm::vec2(1.0F, 0.0F)
                                                                    : glm::vec2(-1.0F, 0.0F);
                    break;
                }
            }
        }
    }
    options.nodeStyleResolver = [](Core::NodeTypeId typeId) {
        static const Render::Theme::NodeVisualRegistry registry{};
        return toRenderStyle(registry.getVisual(typeId));
    };
    options.nodeTitleResolver = [](Core::NodeTypeId typeId) {
        static const Render::Theme::NodeVisualRegistry registry{};
        return registry.getVisual(typeId).title;
    };
    options.nodeIconResolver = [](Core::NodeTypeId typeId) {
        static const Render::Theme::NodeVisualRegistry registry{};
        return registry.getVisual(typeId).icon;
    };
    const float edgePickThresholdSquared = 100.0F / (m_viewState->zoom * m_viewState->zoom);
    for (const auto& route : routes) {
        for (std::size_t i = 0; i + 1 < route.points.size(); ++i) {
            if (distanceToSegmentSquared(mouseWorld, route.points[i], route.points[i + 1]) <= edgePickThresholdSquared) {
                options.hoveredEdgeId = route.edgeId;
                break;
            }
        }
        if (options.hoveredEdgeId.isValid()) {
            break;
        }
    }
    m_renderer.render(m_basics->model(), routes, m_camera, options);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFb);
    glViewport(oldVp[0], oldVp[1], oldVp[2], oldVp[3]);

    ImGui::Image((ImTextureID)(intptr_t)m_colorTexture, canvasSize, ImVec2(0, 1), ImVec2(1, 0));

    const bool hovered = ImGui::IsItemHovered();
    const bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    const bool dragging = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
    const bool released = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    m_hoveredConnector = hovered ? pickConnector(mouseWorld) : Core::CanvasConnectorId{};

    if (hovered && ImGui::GetIO().MouseWheel != 0.0F) {
        m_viewState->zoom = std::max(0.05F, m_viewState->zoom + ImGui::GetIO().MouseWheel * 0.1F);
    }
    if (hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        const ImVec2 delta = ImGui::GetIO().MouseDelta;
        m_viewState->cameraX -= delta.x / m_viewState->zoom;
        m_viewState->cameraY += delta.y / m_viewState->zoom;
    }

    if (clicked) {
        m_dragNode = {};
        m_selectionRectActive = false;

        if (m_hoveredConnector.isValid()) {
            if (m_selectedEdge.isValid()) {
                if (const Core::Edge* edge = m_basics->model().getEdge(m_selectedEdge); edge != nullptr) {
                    if (m_hoveredConnector == edge->fromConnector || m_hoveredConnector == edge->toConnector) {
                        m_reconnectingEdge = true;
                        m_reconnectingFromStart = m_hoveredConnector == edge->fromConnector;
                        m_reconnectOriginalFrom = edge->fromConnector;
                        m_reconnectOriginalTo = edge->toConnector;
                        m_reconnectFixedConnector = m_reconnectingFromStart ? edge->toConnector : edge->fromConnector;
                        m_basics->deleteEdge(m_selectedEdge);
                        m_pendingConnector = m_reconnectFixedConnector;
                    } else {
                        m_pendingConnector = m_hoveredConnector;
                        m_selectedEdge = {};
                    }
                }
            } else {
                m_pendingConnector = m_hoveredConnector;
                m_selectedEdge = {};
            }
        } else {
            const Core::CanvasNodeId pickedNode = pickNode(mouseWorld);
            if (pickedNode.isValid()) {
                if (!m_basics->isNodeSelected(pickedNode)) {
                    m_basics->setSelection({pickedNode});
                }
                m_dragNode = pickedNode;
                m_selectedEdge = {};
                m_reconnectingEdge = false;
                m_dragSelection = m_basics->selectedNodeIds();
                m_dragSelectionStartPositions.clear();
                for (const Core::CanvasNodeId nodeId : m_dragSelection) {
                    const Core::Node* node = m_basics->model().getNode(nodeId);
                    if (node != nullptr) {
                        m_dragSelectionStartPositions.push_back(node->position);
                    }
                }
                if (const Core::Node* node = m_basics->model().getNode(pickedNode); node != nullptr) {
                    m_dragOffset = mouseWorld - node->position;
                    m_dragAnchorStartPosition = node->position;
                }
            } else if (options.hoveredEdgeId.isValid()) {
                m_basics->clearSelection();
                m_selectedEdge = options.hoveredEdgeId;
            } else {
                m_selectedEdge = {};
                m_basics->clearSelection();
                m_selectionRectActive = true;
                m_selectionRectStart = mouseWorld;
                m_selectionRectEnd = mouseWorld;
            }
        }
    }

    if (dragging && m_dragNode.isValid()) {
        const Core::Node* draggedNode = m_basics->model().getNode(m_dragNode);
        if (draggedNode != nullptr) {
            const glm::vec2 base = snapToGrid(mouseWorld - m_dragOffset);
            const glm::vec2 delta = base - m_dragAnchorStartPosition;
            for (std::size_t i = 0; i < m_dragSelection.size() && i < m_dragSelectionStartPositions.size(); ++i) {
                if (Core::Node* node = m_basics->model().getNode(m_dragSelection[i]); node != nullptr) {
                    node->setPosition(snapToGrid(m_dragSelectionStartPositions[i] + delta));
                }
            }
        }
    } else if (dragging && m_selectionRectActive) {
        m_selectionRectEnd = mouseWorld;
        applySelectionRect();
    }

    if (released) {
        if (m_pendingConnector.isValid()) {
            const Core::CanvasConnectorId target = hovered ? pickConnector(mouseWorld) : Core::CanvasConnectorId{};
            Core::CanvasEdgeId newEdge{};
            if (target.isValid() && target != m_pendingConnector &&
                canConnect(m_basics->model(), m_pendingConnector, target)) {
                newEdge = m_reconnectingEdge ? (m_reconnectingFromStart ? m_basics->connect(target, m_reconnectFixedConnector)
                                                                         : m_basics->connect(m_reconnectFixedConnector, target))
                                            : m_basics->connect(m_pendingConnector, target);
            } else if (m_reconnectingEdge) {
                newEdge = m_basics->connect(m_reconnectOriginalFrom, m_reconnectOriginalTo);
            }
            if (newEdge.isValid()) {
                m_selectedEdge = newEdge;
            }
            m_pendingConnector = {};
            m_reconnectingEdge = false;
            m_reconnectFixedConnector = {};
            m_reconnectOriginalFrom = {};
            m_reconnectOriginalTo = {};
        } else if (m_selectionRectActive) {
            applySelectionRect();
            m_selectionRectActive = false;
        }
        m_dragNode = {};
        m_dragSelection.clear();
        m_dragSelectionStartPositions.clear();
    }

    if (hovered && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        if (m_selectedEdge.isValid()) {
            m_basics->deleteEdge(m_selectedEdge);
            m_selectedEdge = {};
        }
        const std::vector<Core::CanvasNodeId> selection = m_basics->selectedNodeIds();
        for (const Core::CanvasNodeId nodeId : selection) {
            m_basics->deleteNode(nodeId);
        }
        m_basics->clearSelection();
    }

    ImGui::End();
}
} // namespace ScopeCanvas::Studio

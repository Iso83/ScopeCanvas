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
        if (edge != nullptr && edge->fromConnector == a && edge->toConnector == b) {
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
    style.borderColor = rgba(visual.borderColor);
    style.selectionColor = rgba(visual.selectionColor);
    style.borderThickness = visual.borderThickness;
    style.headerHeight = visual.titleBarHeight;
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
    const float count = static_cast<float>(node.connectors.size() + 1U);
    const float y = node.position.y + (node.size.y / count) * static_cast<float>(index + 1U);
    const bool right = (index % 2U) == 1U;
    return {right ? node.position.x + node.size.x : node.position.x, y};
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
        if (nodeMin.x >= rectMin.x && nodeMax.x <= rectMax.x && nodeMin.y >= rectMin.y && nodeMax.y <= rectMax.y) {
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

    Render::CanvasRenderOptions options{};
    options.showGrid = m_showGrid && m_basics->gridSettings().enabled;
    options.showDebug = m_showDebug;
    options.gridSize = m_basics->gridSettings().cellSize;
    options.selectedNodeIds = m_basics->selectedNodeIds();
    options.selectionRectActive = m_selectionRectActive;
    options.selectionRectStart = m_selectionRectStart;
    options.selectionRectEnd = m_selectionRectEnd;
    options.hoveredConnectorId = m_hoveredConnector;
    options.activeConnectorId = m_pendingConnector;
    options.previewEdgeActive = m_pendingConnector.isValid();
    options.previewEdgeEnd = screenToWorld(ImGui::GetIO().MousePos.x - canvasPos.x, ImGui::GetIO().MousePos.y - canvasPos.y,
                                           std::max(canvasSize.x, 1.0F), std::max(canvasSize.y, 1.0F));
    if (m_pendingConnector.isValid()) {
        const Core::Connector* connector = m_basics->model().getConnector(m_pendingConnector);
        const Core::Node* node = connector == nullptr ? nullptr : m_basics->model().getNode(connector->nodeId);
        if (node != nullptr) {
            for (std::size_t i = 0; i < node->connectors.size(); ++i) {
                if (node->connectors[i] == m_pendingConnector) {
                    options.previewEdgeStart = connectorWorld(*node, i);
                    break;
                }
            }
        }
    }
    options.nodeStyleResolver = [](Core::NodeTypeId typeId) {
        static const Render::Theme::NodeVisualRegistry registry{};
        return toRenderStyle(registry.getVisual(typeId));
    };
    m_renderer.render(m_basics->model(), m_basics->routeAllEdges(), m_camera, options);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFb);
    glViewport(oldVp[0], oldVp[1], oldVp[2], oldVp[3]);

    ImGui::Image((ImTextureID)(intptr_t)m_colorTexture, canvasSize, ImVec2(0, 1), ImVec2(1, 0));

    const bool hovered = ImGui::IsItemHovered();
    const bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    const bool dragging = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
    const bool released = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    const ImVec2 mouse = ImGui::GetIO().MousePos;
    const glm::vec2 mouseWorld =
        screenToWorld(mouse.x - canvasPos.x, mouse.y - canvasPos.y, std::max(canvasSize.x, 1.0F), std::max(canvasSize.y, 1.0F));
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
            m_pendingConnector = m_hoveredConnector;
        } else {
            const Core::CanvasNodeId pickedNode = pickNode(mouseWorld);
            if (pickedNode.isValid()) {
                if (!m_basics->isNodeSelected(pickedNode)) {
                    m_basics->setSelection({pickedNode});
                }
                m_dragNode = pickedNode;
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
                }
            } else {
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
            const glm::vec2 delta = base - draggedNode->position;
            for (std::size_t i = 0; i < m_dragSelection.size() && i < m_dragSelectionStartPositions.size(); ++i) {
                if (Core::Node* node = m_basics->model().getNode(m_dragSelection[i]); node != nullptr) {
                    node->setPosition(snapToGrid(m_dragSelectionStartPositions[i] + delta));
                }
            }
        }
    } else if (dragging && m_selectionRectActive) {
        m_selectionRectEnd = mouseWorld;
    }

    if (released) {
        if (m_pendingConnector.isValid()) {
            const Core::CanvasConnectorId target = hovered ? pickConnector(mouseWorld) : Core::CanvasConnectorId{};
            if (target.isValid() && target != m_pendingConnector &&
                canConnect(m_basics->model(), m_pendingConnector, target)) {
                (void)m_basics->connect(m_pendingConnector, target);
            }
            m_pendingConnector = {};
        } else if (m_selectionRectActive) {
            applySelectionRect();
            m_selectionRectActive = false;
        }
        m_dragNode = {};
        m_dragSelection.clear();
        m_dragSelectionStartPositions.clear();
    }

    if (hovered && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        const std::vector<Core::CanvasNodeId> selection = m_basics->selectedNodeIds();
        for (const Core::CanvasNodeId nodeId : selection) {
            m_basics->deleteNode(nodeId);
        }
        m_basics->clearSelection();
    }

    ImGui::End();
}
} // namespace ScopeCanvas::Studio

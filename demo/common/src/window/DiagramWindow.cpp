#include <ScopeCanvas/demo/window/DiagramWindow.h>
#include <ScopeCanvas/routing/Geometry.h>
#include <ScopeCanvas/widget/theme/NodeVisualRegistry.h>
#include <algorithm>
#include <cmath>
#include <glad/glad.h>
#include <utility>

using namespace ScopeCanvas::Core;
using namespace ScopeCanvas::Core::Ids;
using namespace ScopeCanvas::Routing;

namespace ScopeCanvas::Demo {

DiagramWindow::DiagramWindow(std::string title, DiagramBasics* basics) : m_title(std::move(title)), m_basics(basics) {}

DiagramWindow::~DiagramWindow() {
    releaseRenderTarget();

    if (m_rendererInitialized)
        m_renderer.shutdown();
}

void DiagramWindow::draw(int width, int height, const DiagramInput& input) {
    GLint oldVp[4]{};
    glGetIntegerv(GL_VIEWPORT, oldVp);
    glViewport(0, 0, std::max(width, 1), std::max(height, 1));
    renderContent(width, height, input);
    glViewport(oldVp[0], oldVp[1], oldVp[2], oldVp[3]);
}

unsigned int DiagramWindow::drawToTexture(int width, int height, const DiagramInput& input) {
    ensureRenderTarget(width, height);

    GLint oldFb = 0;
    GLint oldVp[4]{};
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
    glGetIntegerv(GL_VIEWPORT, oldVp);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_renderWidth, m_renderHeight);
    renderContent(m_renderWidth, m_renderHeight, input);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFb);
    glViewport(oldVp[0], oldVp[1], oldVp[2], oldVp[3]);
    return m_colorTexture;
}

void DiagramWindow::renderContent(int width, int height, const DiagramInput& input) {
    if (m_basics == nullptr)
        return;

    if (!m_rendererInitialized)
        m_rendererInitialized = m_renderer.init();

    width = std::max(width, 1);
    height = std::max(height, 1);
    m_camera.setViewportSize(width, height);

    const glm::vec2 mouseWorld =
        screenToWorld(input.mouseX, input.mouseY, static_cast<float>(width), static_cast<float>(height));

    EdgeRouter router;
    const std::vector<EdgeRoute> routes = router.routeAll(&m_basics->model());

    if (input.hovered)
        m_hoveredConnector = pickConnector(mouseWorld);
    else
        m_hoveredConnector = {};

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
        const Connector* connector = m_basics->model().getConnector(m_pendingConnector);
        const Node* node = connector == nullptr ? nullptr : m_basics->model().getNode(connector->nodeId);
        if (node != nullptr) {
            for (std::size_t i = 0; i < node->connectors.size(); ++i) {
                if (node->connectors[i] == m_pendingConnector) {
                    options.previewEdgeStart = connectorWorld(*node, i);
                    options.previewEdgeStartNormal = (i % 2U) == 1U ? glm::vec2(1.0F, 0.0F) : glm::vec2(-1.0F, 0.0F);
                    break;
                }
            }
        }
    }
    options.nodeStyleResolver = [](NodeTypeId typeId) {
        static const Render::Theme::NodeVisualRegistry registry{};
        return registry.getVisual(typeId).toRenderStyle();
    };
    options.nodeTitleResolver = [](NodeTypeId typeId) {
        static const Render::Theme::NodeVisualRegistry registry{};
        return registry.getVisual(typeId).title;
    };
    options.nodeIconResolver = [](NodeTypeId typeId) {
        static const Render::Theme::NodeVisualRegistry registry{};
        return registry.getVisual(typeId).icon;
    };
    const float edgePickThresholdSquared = 100.0F / (m_camera.zoom() * m_camera.zoom());
    for (const auto& route : routes) {
        for (std::size_t i = 0; i + 1 < route.points.size(); ++i) {
            if (distanceToSegmentSquared(mouseWorld, route.points[i], route.points[i + 1]) <=
                edgePickThresholdSquared) {
                options.hoveredEdgeId = route.edgeId;
                break;
            }
        }
        if (options.hoveredEdgeId.isValid())
            break;
    }

    m_renderer.render(m_basics->model(), routes, m_camera, options);

    if (input.hovered && input.scrollDelta != 0.0F)
        m_camera.setZoom(std::max(0.05F, m_camera.zoom() + input.scrollDelta * 0.1F));

    if (input.hovered && input.middleDown) {
        auto pos = m_camera.position();
        pos.x -= input.mouseDelta.x / m_camera.zoom();
        pos.y += input.mouseDelta.y / m_camera.zoom();
        m_camera.setPosition(pos);
    }

    if (input.hovered && input.leftPressed) {
        m_dragNode = {};
        m_selectionRectActive = false;
        if (m_hoveredConnector.isValid()) {
            if (m_selectedEdge.isValid()) {
                if (const Edge* edge = m_basics->model().getEdge(m_selectedEdge); edge != nullptr) {
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
            const NodeId pickedNode = pickNode(mouseWorld);
            if (pickedNode.isValid()) {
                if (!m_basics->isNodeSelected(pickedNode))
                    m_basics->setSelection({pickedNode});
                m_dragNode = pickedNode;
                m_selectedEdge = {};
                m_reconnectingEdge = false;
                m_dragSelection = m_basics->selectedNodeIds();
                m_dragSelectionStartPositions.clear();
                for (const NodeId nodeId : m_dragSelection) {
                    const Node* node = m_basics->model().getNode(nodeId);
                    if (node != nullptr)
                        m_dragSelectionStartPositions.push_back(node->position);
                }
                if (const Node* node = m_basics->model().getNode(pickedNode); node != nullptr) {
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

    if (input.leftDown && m_dragNode.isValid()) {
        const glm::vec2 base = snapToGrid(mouseWorld - m_dragOffset);
        const glm::vec2 delta = base - m_dragAnchorStartPosition;
        for (std::size_t i = 0; i < m_dragSelection.size() && i < m_dragSelectionStartPositions.size(); ++i) {
            if (Node* node = m_basics->model().getNode(m_dragSelection[i]); node != nullptr)
                node->position = snapToGrid(m_dragSelectionStartPositions[i] + delta);
        }
    } else if (input.leftDown && m_selectionRectActive) {
        m_selectionRectEnd = mouseWorld;
        applySelectionRect();
    }

    if (input.leftReleased) {
        if (m_pendingConnector.isValid()) {
            const ConnectorId target = input.hovered ? pickConnector(mouseWorld) : ConnectorId{};
            EdgeId newEdge{};
            if (target.isValid() && target != m_pendingConnector && m_basics->canConnect(m_pendingConnector, target)) {
                newEdge = m_reconnectingEdge
                              ? (m_reconnectingFromStart ? m_basics->connect(target, m_reconnectFixedConnector)
                                                         : m_basics->connect(m_reconnectFixedConnector, target))
                              : m_basics->connect(m_pendingConnector, target);
            } else if (m_reconnectingEdge)
                newEdge = m_basics->connect(m_reconnectOriginalFrom, m_reconnectOriginalTo);
            if (newEdge.isValid())
                m_selectedEdge = newEdge;
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

    if (input.hovered && input.deletePressed)
        deleteSelection();
}

Core::Ids::NodeId DiagramWindow::createNodeAtCenter(Core::Ids::NodeTypeId typeId) {
    if (m_basics == nullptr)
        return {};

    glm::vec2 position = m_camera.position() - glm::vec2(96.0F, 56.0F);
    position = snapToGrid(position);
    const NodeId nodeId = m_basics->createNode(typeId, position);
    m_basics->setSelection({nodeId});
    m_selectedEdge = {};
    return nodeId;
}

void DiagramWindow::deleteSelection() {
    if (m_basics == nullptr)
        return;

    if (m_selectedEdge.isValid()) {
        m_basics->deleteEdge(m_selectedEdge);
        m_selectedEdge = {};
    }

    const std::vector<NodeId> selection = m_basics->selectedNodeIds();
    for (const NodeId nodeId : selection)
        m_basics->deleteNode(nodeId);

    m_basics->clearSelection();
}

void DiagramWindow::ensureRenderTarget(int width, int height) {
    width = std::max(width, 1);
    height = std::max(height, 1);
    if (m_framebuffer != 0 && m_renderWidth == width && m_renderHeight == height)
        return;

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

void DiagramWindow::releaseRenderTarget() {
    if (m_depthStencilRenderbuffer != 0)
        glDeleteRenderbuffers(1, &m_depthStencilRenderbuffer);

    if (m_colorTexture != 0)
        glDeleteTextures(1, &m_colorTexture);

    if (m_framebuffer != 0)
        glDeleteFramebuffers(1, &m_framebuffer);

    m_depthStencilRenderbuffer = 0;
    m_colorTexture = 0;
    m_framebuffer = 0;
}

glm::vec2 DiagramWindow::snapToGrid(glm::vec2 position) const {
    if (!m_basics->gridSettings().snapEnabled)
        return position;

    const float s = m_basics->gridSettings().cellSize;
    position.x = std::round(position.x / s) * s;
    position.y = std::round(position.y / s) * s;

    return position;
}

glm::vec2 DiagramWindow::screenToWorld(float sx, float sy, float w, float h) const {
    const float nx = (sx / w) * 2.0F - 1.0F;
    const float ny = 1.0F - (sy / h) * 2.0F;
    const glm::mat4 inv = m_camera.invViewProjection();
    const glm::vec4 v = inv * glm::vec4(nx, ny, 0.0F, 1.0F);

    return {v.x / v.w, v.y / v.w};
}

glm::vec2 DiagramWindow::connectorWorld(const Core::Node& node, std::size_t index) const {
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

NodeId DiagramWindow::pickNode(const glm::vec2& world) const {
    for (auto it = m_basics->nodeIds().rbegin(); it != m_basics->nodeIds().rend(); ++it) {
        const Core::Node* node = m_basics->model().getNode(*it);
        if (node != nullptr && pointInNode(world, *node))
            return *it;
    }

    return {};
}

ConnectorId DiagramWindow::pickConnector(const glm::vec2& world) const {
    const float pickRadiusSquared = 100.0F / (m_camera.zoom() * m_camera.zoom());
    for (auto it = m_basics->nodeIds().rbegin(); it != m_basics->nodeIds().rend(); ++it) {
        const Core::Node* node = m_basics->model().getNode(*it);
        if (node == nullptr)
            continue;

        for (std::size_t i = 0; i < node->connectors.size(); ++i) {
            const glm::vec2 delta = world - connectorWorld(*node, i);
            if (glm::dot(delta, delta) <= pickRadiusSquared)
                return node->connectors[i];
        }
    }

    return {};
}

void DiagramWindow::applySelectionRect() {
    const glm::vec2 rectMin(std::min(m_selectionRectStart.x, m_selectionRectEnd.x),
                            std::min(m_selectionRectStart.y, m_selectionRectEnd.y));
    const glm::vec2 rectMax(std::max(m_selectionRectStart.x, m_selectionRectEnd.x),
                            std::max(m_selectionRectStart.y, m_selectionRectEnd.y));

    std::vector<NodeId> selection;
    for (NodeId nodeId : m_basics->nodeIds()) {
        const Node* node = m_basics->model().getNode(nodeId);
        if (node == nullptr)
            continue;

        const glm::vec2 nodeMin = node->position;
        const glm::vec2 nodeMax = node->position + node->size;
        const bool overlapX = nodeMax.x >= rectMin.x && nodeMin.x <= rectMax.x;
        const bool overlapY = nodeMax.y >= rectMin.y && nodeMin.y <= rectMax.y;
        if (overlapX && overlapY)
            selection.push_back(nodeId);
    }

    m_basics->setSelection(selection);
}

} // namespace ScopeCanvas::Demo
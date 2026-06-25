#include <ScopeCanvas/demo/DiagramDrawContext.h>
#include <ScopeCanvas/render/scene/SceneBuilder.h>
#include <ScopeCanvas/routing/EdgeRouter.h>
#include <ScopeCanvas/routing/Geometry.h>
#include <ScopeCanvas/widget/theme/NodeVisualRegistry.h>

using namespace ScopeCanvas::Render;
using namespace ScopeCanvas::Render::Window;
using namespace ScopeCanvas::Core;
using namespace ScopeCanvas::Core::Ids;
using namespace ScopeCanvas::Routing;

namespace ScopeCanvas::Demo {
DiagramDrawCtx::~DiagramDrawCtx() {
    if (m_nodeInfoRendererInitialized)
        m_nodeInfoRenderer.shutdown();
    if (m_rendererInitialized)
        m_renderer.shutdown();
}

void DiagramDrawCtx::draw(Viewport* view) {
    if (view == nullptr)
        return;

    if (!m_rendererInitialized)
        m_rendererInitialized = m_renderer.init();
    if (!m_nodeInfoRendererInitialized)
        m_nodeInfoRendererInitialized = m_nodeInfoRenderer.init();

    const auto cam = view->camera();
    const auto mouseWorld = view->screenToWorld(m_input.mouseX, m_input.mouseY);

    EdgeRouter router;
    const std::vector<EdgeRoute> routes = router.routeAll(&m_basics.model());

    if (m_input.hovered)
        m_hoveredConnector = pickConnector(cam, mouseWorld);
    else
        m_hoveredConnector = {};

    Render::CanvasRenderOptions options{};
    options.showGrid = m_showGrid && m_basics.gridSettings().enabled;
    options.showDebug = m_showDebug;
    options.gridSize = m_basics.gridSettings().cellSize;
    options.selectedNodeIds = m_basics.selectedNodeIds();
    options.selectedEdgeId = m_selectedEdge;
    options.selectionRectActive = m_selectionRectActive;
    options.selectionRectStart = m_selectionRectStart;
    options.selectionRectEnd = m_selectionRectEnd;
    options.hoveredConnectorId = m_hoveredConnector;
    options.activeConnectorId = m_pendingConnector;
    options.previewEdgeActive = m_pendingConnector.isValid();
    options.previewEdgeEnd = mouseWorld;
    if (m_pendingConnector.isValid()) {
        const Connector* connector = m_basics.model().getConnector(m_pendingConnector);
        const Node* node = connector == nullptr ? nullptr : m_basics.model().getNode(connector->nodeId);
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
        return registry.getVisual(typeId).style;
    };
    const float edgePickThresholdSquared = 100.0F / (cam.zoom() * cam.zoom());
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

    m_renderer.render(m_basics.model(), routes, cam, options);
    if (m_nodeInfoRendererInitialized) {
        static const Render::Theme::NodeVisualRegistry registry{};
        const Render::Scene::SceneBuilder sceneBuilder{};
        const Render::Scene::RenderScene scene = sceneBuilder.build(m_basics.model(), routes, cam);
        m_nodeInfoRenderer.render(scene.nodes, cam, registry);
    }
    m_renderer.renderNodeSelectionBorders(m_basics.model(), routes, cam, options);

    if (m_input.hovered && m_input.scrollDelta != 0.0F)
        view->setViewZoom(std::max(0.05F, cam.zoom() + m_input.scrollDelta * 0.1F));

    if (m_input.hovered && m_input.middleDown) {
        auto pos = cam.position();
        pos.x -= m_input.mouseDelta.x / cam.zoom();
        pos.y += m_input.mouseDelta.y / cam.zoom();
        view->setViewPosition(pos);
    }

    if (m_input.hovered && m_input.leftPressed) {
        m_dragNode = {};
        m_selectionRectActive = false;
        if (m_hoveredConnector.isValid()) {
            if (m_selectedEdge.isValid()) {
                if (const Edge* edge = m_basics.model().getEdge(m_selectedEdge); edge != nullptr) {
                    if (m_hoveredConnector == edge->fromConnector || m_hoveredConnector == edge->toConnector) {
                        m_reconnectingEdge = true;
                        m_reconnectingFromStart = m_hoveredConnector == edge->fromConnector;
                        m_reconnectOriginalFrom = edge->fromConnector;
                        m_reconnectOriginalTo = edge->toConnector;
                        m_reconnectFixedConnector = m_reconnectingFromStart ? edge->toConnector : edge->fromConnector;
                        m_basics.deleteEdge(m_selectedEdge);
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
                if (!m_basics.isNodeSelected(pickedNode))
                    m_basics.setSelection({pickedNode});
                m_dragNode = pickedNode;
                m_selectedEdge = {};
                m_reconnectingEdge = false;
                m_dragSelection = m_basics.selectedNodeIds();
                m_dragSelectionStartPositions.clear();
                for (const NodeId nodeId : m_dragSelection) {
                    const Node* node = m_basics.model().getNode(nodeId);
                    if (node != nullptr)
                        m_dragSelectionStartPositions.push_back(node->position);
                }
                if (const Node* node = m_basics.model().getNode(pickedNode); node != nullptr) {
                    m_dragOffset = mouseWorld - node->position;
                    m_dragAnchorStartPosition = node->position;
                }
            } else if (options.hoveredEdgeId.isValid()) {
                m_basics.clearSelection();
                m_selectedEdge = options.hoveredEdgeId;
            } else {
                m_selectedEdge = {};
                m_basics.clearSelection();
                m_selectionRectActive = true;
                m_selectionRectStart = mouseWorld;
                m_selectionRectEnd = mouseWorld;
            }
        }
    }

    if (m_input.leftDown && m_dragNode.isValid()) {
        const glm::vec2 base = snapToGrid(mouseWorld - m_dragOffset);
        const glm::vec2 delta = base - m_dragAnchorStartPosition;
        for (std::size_t i = 0; i < m_dragSelection.size() && i < m_dragSelectionStartPositions.size(); ++i) {
            if (Node* node = m_basics.model().getNode(m_dragSelection[i]); node != nullptr)
                node->position = snapToGrid(m_dragSelectionStartPositions[i] + delta);
        }
    } else if (m_input.leftDown && m_selectionRectActive) {
        m_selectionRectEnd = mouseWorld;
        applySelectionRect();
    }

    if (m_input.leftReleased) {
        if (m_pendingConnector.isValid()) {
            const ConnectorId target = m_input.hovered ? pickConnector(cam, mouseWorld) : ConnectorId{};
            EdgeId newEdge{};
            if (target.isValid() && target != m_pendingConnector && m_basics.canConnect(m_pendingConnector, target)) {
                newEdge = m_reconnectingEdge
                              ? (m_reconnectingFromStart ? m_basics.connect(target, m_reconnectFixedConnector)
                                                         : m_basics.connect(m_reconnectFixedConnector, target))
                              : m_basics.connect(m_pendingConnector, target);
            } else if (m_reconnectingEdge)
                newEdge = m_basics.connect(m_reconnectOriginalFrom, m_reconnectOriginalTo);
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

    if (m_input.hovered && m_input.deletePressed)
        deleteSelection();
}

void DiagramDrawCtx::deleteSelection() {
    if (m_selectedEdge.isValid()) {
        m_basics.deleteEdge(m_selectedEdge);
        clearSelectedEdge();
    }

    const std::vector<NodeId> selection = m_basics.selectedNodeIds();
    for (const NodeId nodeId : selection)
        m_basics.deleteNode(nodeId);

    m_basics.clearSelection();
}

NodeId DiagramDrawCtx::createNodeAtCenter(const Camera::Camera2D& cam, NodeTypeId typeId) {
    glm::vec2 position = cam.position() - glm::vec2(96.0F, 56.0F);
    position = snapToGrid(position);
    const NodeId nodeId = m_basics.createNode(typeId, position);
    m_basics.setSelection({nodeId});
    clearSelectedEdge();
    return nodeId;
}

glm::vec2 DiagramDrawCtx::snapToGrid(glm::vec2 position) const {
    if (!m_basics.gridSettings().snapEnabled)
        return position;

    const float s = m_basics.gridSettings().cellSize;
    position.x = std::round(position.x / s) * s;
    position.y = std::round(position.y / s) * s;

    return position;
}

glm::vec2 DiagramDrawCtx::connectorWorld(const Core::Node& node, std::size_t index) const {
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

NodeId DiagramDrawCtx::pickNode(const glm::vec2& world) const {
    for (auto it = m_basics.nodeIds().rbegin(); it != m_basics.nodeIds().rend(); ++it) {
        const Core::Node* node = m_basics.model().getNode(*it);
        if (node != nullptr && pointInNode(world, *node))
            return *it;
    }

    return {};
}

ConnectorId DiagramDrawCtx::pickConnector(const Render::Camera::Camera2D& cam, const glm::vec2& world) const {
    const float pickRadiusSquared = 100.0F / (cam.zoom() * cam.zoom());
    for (auto it = m_basics.nodeIds().rbegin(); it != m_basics.nodeIds().rend(); ++it) {
        const Core::Node* node = m_basics.model().getNode(*it);
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

void DiagramDrawCtx::applySelectionRect() {
    const glm::vec2 rectMin(std::min(m_selectionRectStart.x, m_selectionRectEnd.x),
                            std::min(m_selectionRectStart.y, m_selectionRectEnd.y));
    const glm::vec2 rectMax(std::max(m_selectionRectStart.x, m_selectionRectEnd.x),
                            std::max(m_selectionRectStart.y, m_selectionRectEnd.y));

    std::vector<NodeId> selection;
    for (NodeId nodeId : m_basics.nodeIds()) {
        const Node* node = m_basics.model().getNode(nodeId);
        if (node == nullptr)
            continue;

        const glm::vec2 nodeMin = node->position;
        const glm::vec2 nodeMax = node->position + node->size;
        const bool overlapX = nodeMax.x >= rectMin.x && nodeMin.x <= rectMax.x;
        const bool overlapY = nodeMax.y >= rectMin.y && nodeMin.y <= rectMax.y;
        if (overlapX && overlapY)
            selection.push_back(nodeId);
    }

    m_basics.setSelection(selection);
}

} // namespace ScopeCanvas::Demo
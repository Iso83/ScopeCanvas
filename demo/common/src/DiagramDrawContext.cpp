#include <ScopeCanvas/demo/DiagramDrawContext.h>
#include <ScopeCanvas/render/scene/SceneBuilder.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>
#include <ScopeCanvas/routing/EdgeRouter.h>
#include <ScopeCanvas/routing/Geometry.h>
#include <ScopeCanvas/widget/render/theme/NodeVisualRegistry.h>

using namespace std;
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

//-------------------------------------------------------------------------
// Rendering
//-------------------------------------------------------------------------
void DiagramDrawCtx::draw(Viewport* view) {
    if (view == nullptr)
        return;

    if (!m_rendererInitialized)
        m_rendererInitialized = m_renderer.init();
    if (!m_nodeInfoRendererInitialized)
        m_nodeInfoRendererInitialized = m_nodeInfoRenderer.init();

    const DrawFrameState frame = frameState(view);
    const vector<EdgeRoute> routes = buildRoutes();
    const CanvasRenderOptions options = buildRenderOptions(frame, routes);

    renderDiagram(frame, routes, options);
    handleActiveInteraction(frame, options);

    m_needsRender = m_pendingConnector.isValid() || m_dragNode.isValid() || m_selectionRectActive || m_reconnectingEdge;
}

vector<EdgeRoute> DiagramDrawCtx::buildRoutes() const {
    EdgeRouter router;
    return router.routeAll(&m_basics.model());
}

CanvasRenderOptions DiagramDrawCtx::buildRenderOptions(const DrawFrameState& frame, const vector<EdgeRoute>& routes) {
    const ConnectorId previousHoveredConnector = m_hoveredConnector;
    m_hoveredConnector = frame.isActiveView ? pickConnector(frame.zoom, frame.mouseWorld) : ConnectorId{};
    if (m_hoveredConnector != previousHoveredConnector)
        markNeedsRender();

    CanvasRenderOptions options{};
    options.showGrid = m_showGrid && m_basics.gridSettings().enabled;
    options.gridSize = m_basics.gridSettings().cellSize;
    if (frame.isActiveView) {
        options.selectedNodeIds = m_basics.selectedNodeIds();
        options.selectedEdgeId = m_selectedEdge;
    }
    options.selectionRectActive = frame.isActiveView && m_selectionRectActive;
    options.selectionRectStart = m_selectionRectStart;
    options.selectionRectEnd = m_selectionRectEnd;
    options.hoveredConnectorId = frame.isActiveView ? m_hoveredConnector : ConnectorId{};
    options.activeConnectorId = frame.isActiveView ? m_pendingConnector : ConnectorId{};
    options.previewEdgeActive = frame.isActiveView && m_pendingConnector.isValid();
    options.previewEdgeEnd = frame.mouseWorld;

    if (m_pendingConnector.isValid()) {
        const Connector* connector = m_basics.model().getConnector(m_pendingConnector);
        const Node* node = connector == nullptr ? nullptr : m_basics.model().getNode(connector->nodeId);
        if (node != nullptr) {
            for (size_t i = 0; i < node->connectors.size(); ++i) {
                if (node->connectors[i] == m_pendingConnector) {
                    options.previewEdgeStart = connectorWorld(*node, i);
                    options.previewEdgeStartNormal = (i % 2U) == 1U ? glm::vec2(1.0F, 0.0F) : glm::vec2(-1.0F, 0.0F);
                    break;
                }
            }
        }
    }

    options.nodeStyleResolver = [](NodeTypeId typeId) {
        static const Widget::Render::Theme::NodeVisualRegistry registry{};
        return registry.getVisual(typeId).style;
    };

    if (frame.isActiveView) {
        const float edgePickThresholdSquared = 100.0F / (frame.zoom * frame.zoom);
        for (const auto& route : routes) {
            const vector<glm::vec2> edgeGeometry = EdgeRenderer::buildEdgeGeometry(route, 18);
            for (size_t i = 0; i + 1 < edgeGeometry.size(); ++i) {
                if (distanceToSegmentSquared(frame.mouseWorld, edgeGeometry[i], edgeGeometry[i + 1]) <=
                    edgePickThresholdSquared) {
                    options.hoveredEdgeId = route.edgeId;
                    break;
                }
            }
            if (options.hoveredEdgeId.isValid())
                break;
        }
    }

    return options;
}

void DiagramDrawCtx::renderDiagram(const DrawFrameState& frame, const vector<EdgeRoute>& routes,
                                   const CanvasRenderOptions& options) {
    CanvasRenderOptions baseOptions = options;
    baseOptions.selectionRectActive = false;

    m_renderer.render(m_basics.model(), routes, frame.camera, baseOptions);

    if (m_nodeInfoRendererInitialized) {
        static const Widget::Render::Theme::NodeVisualRegistry registry{};
        const Render::Scene::SceneBuilder sceneBuilder{};
        const Render::Scene::RenderScene scene = sceneBuilder.build(m_basics.model(), routes, frame.camera);
        m_nodeInfoRenderer.render(scene.nodes, frame.camera, registry);
    }

    m_renderer.renderNodeSelectionBorders(m_basics.model(), routes, frame.camera, options);

    if (options.selectionRectActive)
        m_renderer.renderSelectionRect(frame.camera, options.selectionRectStart, options.selectionRectEnd);
}

//-------------------------------------------------------------------------
// Interaction
//-------------------------------------------------------------------------
void DiagramDrawCtx::handleActiveInteraction(const DrawFrameState& frame, const CanvasRenderOptions& options) {
    if (!frame.isActiveView || frame.handler == nullptr)
        return;

    processActiveCameraPan(frame);
    processActiveMousePressed(frame, options);
    processActiveMouseDrag(frame);
    processActiveMouseReleased(frame);
    processActiveKeyboard(frame);
}

void DiagramDrawCtx::processActiveCameraPan(const DrawFrameState& frame) {
    if (!frame.handler->mouseState(SC_MOUSE_BUTTON_MIDDLE).down)
        return;

    auto pos = frame.camera.position();
    const auto mouse = frame.handler->mouseDeltaPosition();

    pos.x -= mouse.x / frame.camera.zoom();
    pos.y += mouse.y / frame.camera.zoom();

    frame.view->setViewPosition(pos);
}

void DiagramDrawCtx::processActiveMousePressed(const DrawFrameState& frame, const CanvasRenderOptions& options) {
    if (!frame.handler->mouseState(SC_MOUSE_BUTTON_LEFT).pressed())
        return;

    m_dragNode = {};
    m_selectionRectActive = false;

    if (m_hoveredConnector.isValid()) {
        beginConnectorInteraction();
        markNeedsRender();
        return;
    }

    const NodeId pickedNode = pickNode(frame.mouseWorld);
    if (pickedNode.isValid()) {
        beginNodeDrag(pickedNode, frame.mouseWorld);
        markNeedsRender();
        return;
    }

    if (options.hoveredEdgeId.isValid()) {
        beginEdgeSelection(options.hoveredEdgeId);
        markNeedsRender();
        return;
    }

    beginSelectionRect(frame.mouseWorld);
    markNeedsRender();
}

void DiagramDrawCtx::processActiveMouseDrag(const DrawFrameState& frame) {
    if (!frame.handler->mouseState(SC_MOUSE_BUTTON_LEFT).down)
        return;

    if (m_dragNode.isValid()) {
        const glm::vec2 base = snapToGrid(frame.mouseWorld - m_dragOffset);
        const glm::vec2 delta = base - m_dragAnchorStartPosition;

        for (size_t i = 0; i < m_dragSelection.size() && i < m_dragSelectionStartPositions.size(); ++i) {
            if (Node* node = m_basics.model().getNode(m_dragSelection[i]); node != nullptr)
                node->position = snapToGrid(m_dragSelectionStartPositions[i] + delta);
        }

        markNeedsRender();
        return;
    }

    if (m_selectionRectActive) {
        m_selectionRectEnd = frame.mouseWorld;
        applySelectionRect();
        markNeedsRender();
    }
}

void DiagramDrawCtx::processActiveMouseReleased(const DrawFrameState& frame) {
    if (!frame.handler->mouseState(SC_MOUSE_BUTTON_LEFT).released())
        return;

    if (m_pendingConnector.isValid())
        finishConnectorInteraction(frame);
    else if (m_selectionRectActive) {
        applySelectionRect();
        m_selectionRectActive = false;
    }

    clearDragState();
    markNeedsRender();
}

void DiagramDrawCtx::processActiveKeyboard(const DrawFrameState& frame) {
    if (!frame.handler->keyState(ScopeCanvas::Input::Key::Delete).down)
        return;

    deleteSelection();
    markNeedsRender();
}

void DiagramDrawCtx::beginConnectorInteraction() {
    if (!m_selectedEdge.isValid()) {
        m_pendingConnector = m_hoveredConnector;
        m_selectedEdge = {};
        return;
    }

    const Edge* edge = m_basics.model().getEdge(m_selectedEdge);
    if (edge == nullptr)
        return;

    const bool fromStart = m_hoveredConnector == edge->fromConnector;
    const bool fromEnd = m_hoveredConnector == edge->toConnector;

    if (!fromStart && !fromEnd) {
        m_pendingConnector = m_hoveredConnector;
        m_selectedEdge = {};
        return;
    }

    m_reconnectingEdge = true;
    m_reconnectingFromStart = fromStart;
    m_reconnectOriginalFrom = edge->fromConnector;
    m_reconnectOriginalTo = edge->toConnector;
    m_reconnectFixedConnector = fromStart ? edge->toConnector : edge->fromConnector;

    m_basics.deleteEdge(m_selectedEdge);
    m_pendingConnector = m_reconnectFixedConnector;
}

void DiagramDrawCtx::beginNodeDrag(NodeId nodeId, const glm::vec2& mouseWorld) {
    if (!m_basics.isNodeSelected(nodeId))
        m_basics.setSelection({nodeId});

    m_dragNode = nodeId;
    m_selectedEdge = {};
    m_reconnectingEdge = false;

    m_dragSelection = m_basics.selectedNodeIds();
    m_dragSelectionStartPositions.clear();

    for (const NodeId selectedNodeId : m_dragSelection) {
        const Node* node = m_basics.model().getNode(selectedNodeId);
        if (node != nullptr)
            m_dragSelectionStartPositions.push_back(node->position);
    }

    const Node* node = m_basics.model().getNode(nodeId);
    if (node == nullptr)
        return;

    m_dragOffset = mouseWorld - node->position;
    m_dragAnchorStartPosition = node->position;
}

void DiagramDrawCtx::beginEdgeSelection(EdgeId edgeId) {
    m_basics.clearSelection();
    m_selectedEdge = edgeId;
}

void DiagramDrawCtx::beginSelectionRect(const glm::vec2& mouseWorld) {
    m_selectedEdge = {};
    m_basics.clearSelection();

    m_selectionRectActive = true;
    m_selectionRectStart = mouseWorld;
    m_selectionRectEnd = mouseWorld;
}

void DiagramDrawCtx::finishConnectorInteraction(const DrawFrameState& frame) {
    const ConnectorId target = pickConnector(frame.zoom, frame.mouseWorld);

    EdgeId newEdge{};
    if (target.isValid() && target != m_pendingConnector && m_basics.canConnect(m_pendingConnector, target)) {
        newEdge = m_reconnectingEdge ? (m_reconnectingFromStart ? m_basics.connect(target, m_reconnectFixedConnector)
                                                                : m_basics.connect(m_reconnectFixedConnector, target))
                                     : m_basics.connect(m_pendingConnector, target);
    } else if (m_reconnectingEdge) {
        newEdge = m_basics.connect(m_reconnectOriginalFrom, m_reconnectOriginalTo);
    }

    if (newEdge.isValid())
        m_selectedEdge = newEdge;

    m_pendingConnector = {};
    clearReconnectState();
}

void DiagramDrawCtx::clearDragState() {
    m_dragNode = {};
    m_dragSelection.clear();
    m_dragSelectionStartPositions.clear();
}

void DiagramDrawCtx::clearReconnectState() {
    m_reconnectingEdge = false;
    m_reconnectFixedConnector = {};
    m_reconnectOriginalFrom = {};
    m_reconnectOriginalTo = {};
}

//-------------------------------------------------------------------------
// Interaction: Selection
//-------------------------------------------------------------------------
void DiagramDrawCtx::deleteSelection() {
    if (m_selectedEdge.isValid()) {
        m_basics.deleteEdge(m_selectedEdge);
        clearSelectedEdge();
    }

    const vector<NodeId> selection = m_basics.selectedNodeIds();
    for (const NodeId nodeId : selection)
        m_basics.deleteNode(nodeId);

    m_basics.clearSelection();
    markNeedsRender();
}

//-------------------------------------------------------------------------
// State
//-------------------------------------------------------------------------
void DiagramDrawCtx::markNeedsRender() {
    m_needsRender = true;
}

DiagramDrawCtx::DrawFrameState DiagramDrawCtx::frameState(Viewport* view) const {
    DrawFrameState frame{};
    frame.view = view;
    frame.handler = view->handler();
    frame.isActiveView = frame.handler != nullptr && frame.handler->activeViewport() == view;
    frame.zoom = view->viewZoom();
    frame.camera = view->camera();

    if (frame.handler != nullptr) {
        const auto mousePos = frame.handler->mousePosition();
        frame.mouseWorld = view->screenToWorld(mousePos.x, mousePos.y);
    }

    return frame;
}

//-------------------------------------------------------------------------
// Helpers
//-------------------------------------------------------------------------
NodeId DiagramDrawCtx::createNodeAtCenter(const Camera::Camera2D& cam, NodeTypeId typeId) {
    glm::vec2 position = cam.position() - glm::vec2(96.0F, 56.0F);
    position = snapToGrid(position);
    const NodeId nodeId = m_basics.createNode(typeId, position);
    m_basics.setSelection({nodeId});
    clearSelectedEdge();
    markNeedsRender();
    return nodeId;
}

//-------------------------------------------------------------------------
// Helpers: Geometry
//-------------------------------------------------------------------------
glm::vec2 DiagramDrawCtx::snapToGrid(glm::vec2 position) const {
    if (!m_basics.gridSettings().snapEnabled)
        return position;

    const float s = m_basics.gridSettings().cellSize;
    position.x = round(position.x / s) * s;
    position.y = round(position.y / s) * s;

    return position;
}

glm::vec2 DiagramDrawCtx::connectorWorld(const Core::Node& node, size_t index) const {
    const bool output = (index % 2U) == 1U;
    const size_t sideIndex = index / 2U;
    const size_t sideCount = output ? node.connectors.size() / 2U : (node.connectors.size() + 1U) / 2U;
    constexpr float headerHeight = 24.0F;
    constexpr float verticalInset = 12.0F;
    const float bodyMinY = node.position.y + verticalInset;
    const float bodyMaxY = max(bodyMinY + 1.0F, node.position.y + node.size.y - headerHeight - verticalInset);
    const float bodyHeight = max(bodyMaxY - bodyMinY, 1.0F);
    const float step = bodyHeight / static_cast<float>(sideCount + 1U);
    const float y = bodyMinY + step * static_cast<float>(sideIndex + 1U);

    return {output ? node.position.x + node.size.x : node.position.x, y};
}

//-------------------------------------------------------------------------
// Helpers: Picking
//-------------------------------------------------------------------------
NodeId DiagramDrawCtx::pickNode(const glm::vec2& world) const {
    for (auto it = m_basics.nodeIds().rbegin(); it != m_basics.nodeIds().rend(); ++it) {
        const Core::Node* node = m_basics.model().getNode(*it);
        if (node != nullptr && pointInNode(world, *node))
            return *it;
    }

    return {};
}

ConnectorId DiagramDrawCtx::pickConnector(const float camZoom, const glm::vec2& world) const {
    const float pickRadiusSquared = 100.0F / (camZoom * camZoom);
    for (auto it = m_basics.nodeIds().rbegin(); it != m_basics.nodeIds().rend(); ++it) {
        const Core::Node* node = m_basics.model().getNode(*it);
        if (node == nullptr)
            continue;

        for (size_t i = 0; i < node->connectors.size(); ++i) {
            const glm::vec2 delta = world - connectorWorld(*node, i);
            if (glm::dot(delta, delta) <= pickRadiusSquared)
                return node->connectors[i];
        }
    }

    return {};
}

//-------------------------------------------------------------------------
// Helpers: Selection
//-------------------------------------------------------------------------
void DiagramDrawCtx::applySelectionRect() {
    const glm::vec2 rectMin(min(m_selectionRectStart.x, m_selectionRectEnd.x),
                            min(m_selectionRectStart.y, m_selectionRectEnd.y));
    const glm::vec2 rectMax(max(m_selectionRectStart.x, m_selectionRectEnd.x),
                            max(m_selectionRectStart.y, m_selectionRectEnd.y));

    vector<NodeId> selection;
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
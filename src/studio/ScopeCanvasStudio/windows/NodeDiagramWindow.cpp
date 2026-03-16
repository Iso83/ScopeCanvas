#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "NodeDiagramWindow.h"

#include "Interaction/Commands/GraphCommands.h"

#include <glm/vec4.hpp>

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace {
ImU32 styleColorForNode(const Node &node) {
    if (node.allowChildren) {
        return IM_COL32(80, 120, 190, 120);
    }
    if (node.nodeTypeId == "Number") {
        return IM_COL32(170, 120, 70, 130);
    }

    return IM_COL32(130, 130, 130, 95);
}

ImU32 connectorStateColor(const Node &node, const Connector &connector) {
    if (connector.direction == ConnectorDirection::Input) {
        return IM_COL32(215, 215, 230, 255);
    }

    if (node.nodeTypeId == "Number") {
        return IM_COL32(255, 180, 90, 255);
    }

    return IM_COL32(225, 225, 240, 255);
}

ImVec2 worldToCanvasScreen(const Camera2D &camera, const glm::vec2 &world, const ImVec2 &canvasPos, const ImVec2 &canvasSize) {
    const glm::vec4 p = camera.viewProjection() * glm::vec4(world, 0.0f, 1.0f);
    if (p.w == 0.0f) {
        return canvasPos;
    }

    const float ndcX = p.x / p.w;
    const float ndcY = p.y / p.w;
    const float sx = canvasPos.x + ((ndcX + 1.0f) * 0.5f) * canvasSize.x;
    const float sy = canvasPos.y + ((1.0f - ndcY) * 0.5f) * canvasSize.y;
    return { sx, sy };
}

Node *hitTestNodeByWorld(DiagramModel &graph, const glm::vec2 &mouseWorld) {
    for (auto it = graph.nodes().rbegin(); it != graph.nodes().rend(); ++it) {
        Node &node = *it;
        if (mouseWorld.x >= node.position.x &&
            mouseWorld.x <= node.position.x + node.size.x &&
            mouseWorld.y >= node.position.y &&
            mouseWorld.y <= node.position.y + node.size.y) {
            return &node;
        }
    }

    return nullptr;
}


const ParentLayout *findParentLayoutForChild(const DiagramBasics &basics, uint32_t childNodeId) {
    for (const auto &entry : basics.parentLayouts()) {
        const ParentLayout &layout = entry.second;
        for (const ParentLayoutSlot &slot : layout.slots) {
            if (slot.childNodeId == childNodeId) {
                return &layout;
            }
        }
    }

    return nullptr;
}

glm::vec2 effectiveConnectorWorld(const DiagramBasics &basics, const DiagramModel &graph, const Node &node, const Connector &connector) {
    const ParentLayout *layout = findParentLayoutForChild(basics, node.id);
    if (layout == nullptr) {
        return connectorWorldPosition(node, connector);
    }

    const Node *parent = graph.findNode(layout->parentNodeId);
    if (parent == nullptr) {
        return connectorWorldPosition(node, connector);
    }

    const bool childInsideParent =
        node.position.x >= parent->position.x &&
        node.position.y >= parent->position.y &&
        node.position.x + node.size.x <= parent->position.x + parent->size.x &&
        node.position.y + node.size.y <= parent->position.y + parent->size.y;

    if (!childInsideParent) {
        return connectorWorldPosition(node, connector);
    }

    float railOffset = 0.5f;
    if (connector.direction == ConnectorDirection::Input) {
        int inputIndex = 0;
        int inputCount = 0;
        for (const Connector &c : node.connectors) {
            if (c.direction == ConnectorDirection::Input) {
                ++inputCount;
            }
        }
        for (const Connector &c : node.connectors) {
            if (c.direction != ConnectorDirection::Input) {
                continue;
            }
            if (c.id == connector.id) {
                break;
            }
            ++inputIndex;
        }
        railOffset = static_cast<float>(inputIndex + 1) / static_cast<float>(std::max(1, inputCount + 1));
        Connector proxy{ 0, parent->id, ConnectorSide::Left, railOffset, ConnectorDirection::Input, 8 };
        return connectorWorldPosition(*parent, proxy);
    }

    int outputIndex = 0;
    int outputCount = 0;
    for (const Connector &c : node.connectors) {
        if (c.direction == ConnectorDirection::Output) {
            ++outputCount;
        }
    }
    for (const Connector &c : node.connectors) {
        if (c.direction != ConnectorDirection::Output) {
            continue;
        }
        if (c.id == connector.id) {
            break;
        }
        ++outputIndex;
    }
    railOffset = static_cast<float>(outputIndex + 1) / static_cast<float>(std::max(1, outputCount + 1));
    Connector proxy{ 0, parent->id, ConnectorSide::Right, railOffset, ConnectorDirection::Output, 8 };
    return connectorWorldPosition(*parent, proxy);
}

}

NodeDiagramWindow::NodeDiagramWindow(
    GLFWwindow *window,
    Renderer *renderer,
    DiagramBasics *basics,
    GraphView *view,
    std::string windowTitle)
    : m_window(window),
      m_renderer(renderer),
      m_basics(basics),
      m_view(view),
      m_windowTitle(std::move(windowTitle)) {
}

NodeDiagramWindow::~NodeDiagramWindow() {
    releaseRenderTarget();
}

void NodeDiagramWindow::handleShortcuts(bool focused) {
    if (!focused || m_basics == nullptr) {
        return;
    }

    ImGuiIO &io = ImGui::GetIO();
    const bool ctrl = io.KeyCtrl;
    const bool shift = io.KeyShift;

    if (ctrl && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
        if (shift) {
            (void)m_basics->engine().commands().redo();
        }
        else {
            (void)m_basics->engine().commands().undo();
        }
    }

    if (ctrl && ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
        (void)m_basics->engine().commands().redo();
    }

    if (ctrl && ImGui::IsKeyPressed(ImGuiKey_D, false)) {
        m_basics->duplicateSelected();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
        m_basics->deleteSelected();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_G, false)) {
        if (shift) {
            m_basics->gridSettings().snapEnabled = !m_basics->gridSettings().snapEnabled;
        }
        else {
            m_basics->gridSettings().enabled = !m_basics->gridSettings().enabled;
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_1, false)) {
        m_basics->createNode("Number", m_view->cameraPosition + glm::vec2(-140.0f, 0.0f));
    }
    if (ImGui::IsKeyPressed(ImGuiKey_2, false)) {
        m_basics->createNode("Add", m_view->cameraPosition + glm::vec2(0.0f, 0.0f));
    }
    if (ImGui::IsKeyPressed(ImGuiKey_3, false)) {
        m_basics->createNode("Multiply", m_view->cameraPosition + glm::vec2(140.0f, 0.0f));
    }
    if (ImGui::IsKeyPressed(ImGuiKey_4, false)) {
        m_basics->createNode("Output", m_view->cameraPosition + glm::vec2(280.0f, 0.0f));
    }

    if (ImGui::IsKeyPressed(ImGuiKey_R, false)) {
        m_basics->alignSelectedConnectors();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_C, false)) {
        m_basics->toggleSelectedGroupsCollapsed();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Equal, false)) {
        m_basics->addOutputConnectorToSelection();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Minus, false)) {
        m_basics->removeLastConnectorFromSelection();
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
        m_depthStencilRenderbuffer = 0;
    }

    if (m_colorTexture != 0) {
        glDeleteTextures(1, &m_colorTexture);
        m_colorTexture = 0;
    }

    if (m_framebuffer != 0) {
        glDeleteFramebuffers(1, &m_framebuffer);
        m_framebuffer = 0;
    }

    m_renderWidth = 0;
    m_renderHeight = 0;
}

glm::vec2 NodeDiagramWindow::screenToWorld(float localX, float localY) const {
    const float width = static_cast<float>(m_renderer->viewportWidth());
    const float height = static_cast<float>(m_renderer->viewportHeight());

    const float ndcX = (localX / width) * 2.0f - 1.0f;
    const float ndcY = 1.0f - (localY / height) * 2.0f;

    const glm::mat4 invViewProjection = m_renderer->camera().invViewProjection();
    const glm::vec4 world = invViewProjection * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
    return glm::vec2(world) / world.w;
}

void NodeDiagramWindow::Draw() {
    if (m_basics == nullptr) {
        return;
    }

    DiagramModel &graph = m_basics->engine().graph();
    CommandManager &commands = m_basics->engine().commands();
    GridSettings &gridSettings = m_basics->gridSettings();

    m_basics->applyParentLayouts();

    ImGui::Begin(m_windowTitle.c_str());

    const bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    handleShortcuts(focused);

    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvasSize.x = std::max(canvasSize.x, 1.0f);
    canvasSize.y = std::max(canvasSize.y, 1.0f);

    ensureRenderTarget(static_cast<int>(canvasSize.x), static_cast<int>(canvasSize.y));

    ImGui::InvisibleButton((m_windowTitle + "_canvas").c_str(),
        canvasSize,
        ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle);

    const bool hovered = ImGui::IsItemHovered();

    const ImVec2 mousePos = ImGui::GetMousePos();
    m_input.mouseX = static_cast<double>(mousePos.x - canvasPos.x);
    m_input.mouseY = static_cast<double>(mousePos.y - canvasPos.y);

    const bool leftDownNow = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
    const bool middleDownNow = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Middle);

    m_renderer->camera().setPosition(m_view->cameraPosition);
    m_renderer->camera().setZoom(m_view->zoom);
    m_renderer->resize(m_renderWidth, m_renderHeight);

    const glm::vec2 mouseWorld = screenToWorld(static_cast<float>(m_input.mouseX), static_cast<float>(m_input.mouseY));
    const float zoom = m_renderer->camera().zoom();

    Node *hoveredNode = hovered ? hitTestNodeByWorld(graph, mouseWorld) : nullptr;
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        if (hoveredNode != nullptr) {
            for (Node &node : graph.nodes()) {
                node.selected = (node.id == hoveredNode->id);
            }
            ImGui::OpenPopup("NodeConnectorPopup");
        }
    }

    if (ImGui::BeginPopup("NodeConnectorPopup")) {
        if (ImGui::MenuItem("Add Input Connector")) {
            m_basics->addInputConnectorToSelection();
        }
        if (ImGui::MenuItem("Add Output Connector")) {
            m_basics->addOutputConnectorToSelection();
        }
        if (ImGui::MenuItem("Delete Last Connector")) {
            m_basics->removeLastConnectorFromSelection();
        }
        ImGui::EndPopup();
    }

    m_hoveredEdgeId = 0;
    m_hoveredConnectorId = 0;

    if (hovered) {
        const Edge *hoveredEdge = EdgeInteractionController::hitTestEdge(graph, mouseWorld, zoom);
        m_hoveredEdgeId = hoveredEdge != nullptr ? hoveredEdge->id.value : 0;

        const Connector *hoveredConnector =
            EdgeInteractionController::hitTestConnector(graph, mouseWorld, zoom, nullptr);
        m_hoveredConnectorId = hoveredConnector != nullptr ? hoveredConnector->id.value : 0;
    }

    if (!m_input.leftDown && leftDownNow) {
        const bool shiftDown = ImGui::GetIO().KeyShift;
        const bool altDown = ImGui::GetIO().KeyAlt;

        const bool startConnect = m_connectController.onMouseDown(graph, mouseWorld, zoom, altDown);
        if (!startConnect) {
            m_selectionController.onMouseDown(graph, mouseWorld, shiftDown);
            m_dragController.onMouseDown(graph, mouseWorld);
        }
    }

    if (m_input.leftDown && !leftDownNow) {
        const ConnectController::ConnectionResult connectResult =
            m_connectController.onMouseUp(graph, mouseWorld, zoom);
        if (connectResult.removeEdge) {
            auto deleteEdgeCommand = std::make_unique<DeleteEdgeCommand>(graph, connectResult.edgeToRemoveId);
            commands.execute(std::move(deleteEdgeCommand));
        }
        if (connectResult.createEdge) {
            auto createEdgeCommand = std::make_unique<CreateEdgeCommand>(graph, connectResult.edge);
            commands.execute(std::move(createEdgeCommand));
        }

        if (!connectResult.handled) {
            std::vector<MoveNodesCommand::MoveItem> moveItems = m_dragController.onMouseUp(graph);

            if (gridSettings.snapEnabled) {
                const float cellSize = std::max(gridSettings.cellSize, 1.0f);
                for (MoveNodesCommand::MoveItem &moveItem : moveItems) {
                    const float snappedX = std::round(moveItem.endPosition.x / cellSize) * cellSize;
                    const float snappedY = std::round(moveItem.endPosition.y / cellSize) * cellSize;
                    moveItem.endPosition = { snappedX, snappedY };

                    Node *node = graph.findNode(moveItem.nodeId);
                    if (node != nullptr) {
                        node->position = moveItem.endPosition;
                        graph.recomputeRoutesForNode(node->id);
                    }
                }

                std::erase_if(moveItems, [](const MoveNodesCommand::MoveItem &moveItem) {
                    return moveItem.startPosition == moveItem.endPosition;
                });
            }

            if (!moveItems.empty()) {
                auto moveCommand = std::make_unique<MoveNodesCommand>(graph, moveItems);
                commands.execute(std::move(moveCommand));
            }

            m_selectionController.onMouseUp(graph, mouseWorld);
        }
    }

    m_input.leftDown = leftDownNow;
    m_input.middleDown = middleDownNow;

    if (hovered) {
        m_input.scrollDelta = ImGui::GetIO().MouseWheel;
    }
    else {
        m_input.scrollDelta = 0.0f;
    }

    if (m_input.leftDown && !m_connectController.isConnecting()) {
        m_dragController.update(graph, mouseWorld);
        if (!m_dragController.isDragging()) {
            m_selectionController.onMouseDrag(graph, mouseWorld);
        }
    }

    m_connectController.onMouseMove(mouseWorld);
    m_basics->applyParentLayouts();
    m_cameraController.update(m_renderer->camera(), m_input);
    m_view->cameraPosition = m_renderer->camera().position();
    m_view->zoom = m_renderer->camera().zoom();
    m_input.scrollDelta = 0.0f;

    const Viewport vp{
        .hoveredEdgeId = m_hoveredEdgeId,
        .hoveredConnectorId = m_hoveredConnectorId,
        .selectionRectActive = m_selectionController.isBoxSelecting(),
        .selectionRectStart = m_selectionController.boxStart(),
        .selectionRectEnd = m_selectionController.boxEnd(),
        .previewActive = m_connectController.isConnecting(),
        .previewStartNode = m_connectController.startNodeId(),
        .previewStartConnector = m_connectController.startConnectorId(),
        .previewPosition = m_connectController.previewPosition()
    };

    GLint oldFramebuffer = 0;
    GLint oldViewport[4];
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFramebuffer);
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_renderWidth, m_renderHeight);
    glDisable(GL_SCISSOR_TEST);

    m_renderer->render(graph, *m_view, vp, gridSettings.enabled, gridSettings.cellSize, false, false);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFramebuffer);
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::Image(
        (ImTextureID)m_colorTexture,
        canvasSize,
        ImVec2(0.0f, 1.0f),
        ImVec2(1.0f, 0.0f));

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    for (const Edge &edge : graph.edges()) {
        const Node *fromNode = graph.findNode(edge.fromNode);
        const Node *toNode = graph.findNode(edge.toNode);
        const Connector *fromConnector = graph.findConnector(edge.fromNode, edge.fromConnector);
        const Connector *toConnector = graph.findConnector(edge.toNode, edge.toConnector);
        if (fromNode == nullptr || toNode == nullptr || fromConnector == nullptr || toConnector == nullptr) {
            continue;
        }

        const glm::vec2 p0w = effectiveConnectorWorld(*m_basics, graph, *fromNode, *fromConnector);
        const glm::vec2 p3w = effectiveConnectorWorld(*m_basics, graph, *toNode, *toConnector);
        const ImVec2 p0 = worldToCanvasScreen(m_renderer->camera(), p0w, canvasPos, canvasSize);
        const ImVec2 p3 = worldToCanvasScreen(m_renderer->camera(), p3w, canvasPos, canvasSize);

        const ImU32 edgeColor = edge.selected ? IM_COL32(255, 210, 120, 220) : IM_COL32(220, 220, 235, 200);
        if (m_basics->viewSettings().curvedEdgeOverlay) {
            const float dx = std::abs(p3.x - p0.x);
            const float c = std::max(80.0f, dx * 0.45f);
            const ImVec2 p1(p0.x + c, p0.y);
            const ImVec2 p2(p3.x - c, p3.y);
            drawList->AddBezierCubic(p0, p1, p2, p3, edgeColor, 2.0f, 20);
        }
        else {
            const float midX = (p0.x + p3.x) * 0.5f;
            drawList->AddLine(p0, ImVec2(midX, p0.y), edgeColor, 2.0f);
            drawList->AddLine(ImVec2(midX, p0.y), ImVec2(midX, p3.y), edgeColor, 2.0f);
            drawList->AddLine(ImVec2(midX, p3.y), p3, edgeColor, 2.0f);
        }
    }
    if (m_basics->viewSettings().shaNodeStyling) {
        for (const Node &node : graph.nodes()) {
            const ImVec2 a = worldToCanvasScreen(m_renderer->camera(), node.position, canvasPos, canvasSize);
            const ImVec2 b = worldToCanvasScreen(m_renderer->camera(), node.position + node.size, canvasPos, canvasSize);

            const ImVec2 minPt(std::min(a.x, b.x), std::min(a.y, b.y));
            const ImVec2 maxPt(std::max(a.x, b.x), std::max(a.y, b.y));
            drawList->AddRectFilled(minPt, maxPt, styleColorForNode(node), 4.0f);
            drawList->AddRect(minPt, maxPt, IM_COL32(235, 235, 240, 180), 4.0f, 0, 1.0f);
            drawList->AddRectFilled(minPt, ImVec2(maxPt.x, minPt.y + 22.0f), IM_COL32(30, 34, 44, 185), 4.0f);

            drawList->AddText(ImVec2(minPt.x + 6.0f, minPt.y + 4.0f), IM_COL32(245, 245, 245, 255), node.title.c_str());

            const bool containerNode = node.allowChildren;
            if (containerNode && hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                const ImVec2 m = ImGui::GetMousePos();
                const ImVec2 arrowCenter(maxPt.x - 12.0f, minPt.y + 11.0f);
                if (std::abs(m.x - arrowCenter.x) < 10.0f && std::abs(m.y - arrowCenter.y) < 10.0f) {
                    m_basics->engine().graph().setNodeCollapsed(node.id, !node.collapsed);
                }
            }

            if (containerNode) {
                const bool collapsed = node.collapsed;
                const ImVec2 arrowCenter(maxPt.x - 12.0f, minPt.y + 11.0f);
                if (collapsed) {
                    drawList->AddTriangleFilled(ImVec2(arrowCenter.x - 3.0f, arrowCenter.y - 4.0f),
                        ImVec2(arrowCenter.x - 3.0f, arrowCenter.y + 4.0f),
                        ImVec2(arrowCenter.x + 3.0f, arrowCenter.y), IM_COL32(245, 245, 245, 220));
                }
                else {
                    drawList->AddTriangleFilled(ImVec2(arrowCenter.x - 4.0f, arrowCenter.y - 3.0f),
                        ImVec2(arrowCenter.x + 4.0f, arrowCenter.y - 3.0f),
                        ImVec2(arrowCenter.x, arrowCenter.y + 3.0f), IM_COL32(245, 245, 245, 220));
                }
            }

            if (m_basics->viewSettings().connectorStateColors) {
                for (const Connector &connector : node.connectors) {
                    const glm::vec2 world = effectiveConnectorWorld(*m_basics, graph, node, connector);
                    const ImVec2 s = worldToCanvasScreen(m_renderer->camera(), world, canvasPos, canvasSize);
                    drawList->AddCircleFilled(s, 4.0f, connectorStateColor(node, connector));
                }
            }
        }
    }


    // parent-child function links (dash-dot style, straight)
    for (const auto &entry : m_basics->parentLayouts()) {
        const ParentLayout &layout = entry.second;
        const Node *parent = graph.findNode(layout.parentNodeId);
        if (parent == nullptr) {
            continue;
        }

        const ImVec2 pTop = worldToCanvasScreen(m_renderer->camera(), parent->position + glm::vec2(parent->size.x * 0.5f, 0.0f), canvasPos, canvasSize);
        const ImVec2 pBottom = worldToCanvasScreen(m_renderer->camera(), parent->position + glm::vec2(parent->size.x * 0.5f, parent->size.y), canvasPos, canvasSize);

        for (size_t i = 0; i < layout.slots.size(); ++i) {
            const uint32_t childId = layout.slots[i].childNodeId;
            if (childId == 0) {
                continue;
            }

            const Node *child = graph.findNode(childId);
            if (child == nullptr) {
                continue;
            }

            const bool childInsideParent =
                child->position.x >= parent->position.x &&
                child->position.y >= parent->position.y &&
                child->position.x + child->size.x <= parent->position.x + parent->size.x &&
                child->position.y + child->size.y <= parent->position.y + parent->size.y;
            if (childInsideParent) {
                continue;
            }

            const ImVec2 cTop = worldToCanvasScreen(m_renderer->camera(), child->position + glm::vec2(child->size.x * 0.5f, 0.0f), canvasPos, canvasSize);
            const ImVec2 cBottom = worldToCanvasScreen(m_renderer->camera(), child->position + glm::vec2(child->size.x * 0.5f, child->size.y), canvasPos, canvasSize);

            const bool useTop = std::abs(cTop.y - pTop.y) < std::abs(cBottom.y - pBottom.y);
            const ImVec2 a = useTop ? pTop : pBottom;
            const ImVec2 b = useTop ? cTop : cBottom;

            const int segments = 14;
            for (int seg = 0; seg < segments; ++seg) {
                if (seg % 3 == 2) {
                    continue;
                }
                const float t0 = static_cast<float>(seg) / static_cast<float>(segments);
                const float t1 = static_cast<float>(seg + 1) / static_cast<float>(segments);
                const ImVec2 s0(a.x + (b.x - a.x) * t0, a.y + (b.y - a.y) * t0);
                const ImVec2 s1(a.x + (b.x - a.x) * t1, a.y + (b.y - a.y) * t1);
                drawList->AddLine(s0, s1, IM_COL32(140, 170, 220, 190), 1.5f);
            }
        }
    }

    ImGui::End();
}

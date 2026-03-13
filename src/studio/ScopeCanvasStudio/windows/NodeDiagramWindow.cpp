#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "NodeDiagramWindow.h"

#include "Interaction/Commands/GraphCommands.h"

#include <glm/vec4.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <utility>

namespace {
const Connector *findConnectorByDirection(const Node *node, ConnectorDirection direction) {
    if (node == nullptr) {
        return nullptr;
    }

    for (const Connector &connector : node->connectors) {
        if (connector.direction == direction) {
            return &connector;
        }
    }

    return nullptr;
}
}

void NodeDiagramWindow::SeedDemoGraph(DiagramModel &graph) {
    graph.clear();

    Node *node1 = graph.createNodeOfType("Number", { -280.0f, -40.0f }, { 180.0f, 100.0f });
    Node *node2 = graph.createNodeOfType("Add", { 0.0f, 90.0f }, { 220.0f, 120.0f });
    Node *node3 = graph.createNodeOfType("Output", { 260.0f, -150.0f }, { 200.0f, 110.0f });

    const Connector *node1Output = findConnectorByDirection(node1, ConnectorDirection::Output);
    const Connector *node2Input = findConnectorByDirection(node2, ConnectorDirection::Input);
    const Connector *node2Output = findConnectorByDirection(node2, ConnectorDirection::Output);
    const Connector *node3Input = findConnectorByDirection(node3, ConnectorDirection::Input);

    if (node1 != nullptr && node2 != nullptr && node1Output != nullptr && node2Input != nullptr) {
        graph.createEdge(node1->id, node1Output->id, node2->id, node2Input->id);
    }

    if (node2 != nullptr && node3 != nullptr && node2Output != nullptr && node3Input != nullptr) {
        graph.createEdge(node2->id, node2Output->id, node3->id, node3Input->id);
    }

    graph.syncIdCounters();
}

NodeDiagramWindow::NodeDiagramWindow(
    GLFWwindow *window,
    Renderer *renderer,
    DiagramModel *graph,
    CommandManager *commands,
    GridSettings *gridSettings,
    GraphView *view,
    std::string windowTitle)
    : m_window(window),
      m_renderer(renderer),
      m_graph(graph),
      m_commands(commands),
      m_gridSettings(gridSettings),
      m_view(view),
      m_windowTitle(std::move(windowTitle)) {
}

NodeDiagramWindow::~NodeDiagramWindow() {
    releaseRenderTarget();
}

void NodeDiagramWindow::handleShortcuts(bool focused) {
    if (!focused || m_commands == nullptr || m_gridSettings == nullptr) {
        return;
    }

    ImGuiIO &io = ImGui::GetIO();
    const bool ctrl = io.KeyCtrl;
    const bool shift = io.KeyShift;

    if (ctrl && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
        if (shift) {
            (void)m_commands->redo();
        }
        else {
            (void)m_commands->undo();
        }
    }

    if (ctrl && ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
        (void)m_commands->redo();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_G, false)) {
        if (shift) {
            m_gridSettings->snapEnabled = !m_gridSettings->snapEnabled;
        }
        else {
            m_gridSettings->enabled = !m_gridSettings->enabled;
        }
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

    m_hoveredEdgeId = 0;
    m_hoveredConnectorId = 0;

    if (hovered) {
        const Edge *hoveredEdge = EdgeInteractionController::hitTestEdge(*m_graph, mouseWorld, zoom);
        m_hoveredEdgeId = hoveredEdge != nullptr ? hoveredEdge->id : 0;

        const Connector *hoveredConnector =
            EdgeInteractionController::hitTestConnector(*m_graph, mouseWorld, zoom, nullptr);
        m_hoveredConnectorId = hoveredConnector != nullptr ? hoveredConnector->id : 0;
    }

    if (!m_input.leftDown && leftDownNow) {
        const bool shiftDown = ImGui::GetIO().KeyShift;
        const bool altDown = ImGui::GetIO().KeyAlt;

        const bool startConnect = m_connectController.onMouseDown(*m_graph, mouseWorld, zoom, altDown);
        if (!startConnect) {
            m_selectionController.onMouseDown(*m_graph, mouseWorld, shiftDown);
            m_dragController.onMouseDown(*m_graph, mouseWorld);
        }
    }

    if (m_input.leftDown && !leftDownNow) {
        const ConnectController::ConnectionResult connectResult =
            m_connectController.onMouseUp(*m_graph, mouseWorld, zoom);
        if (connectResult.removeEdge) {
            if (m_commands != nullptr) {
                auto deleteEdgeCommand = std::make_unique<DeleteEdgeCommand>(*m_graph, connectResult.edgeToRemoveId);
                m_commands->execute(std::move(deleteEdgeCommand));
            }
            else {
                m_graph->removeEdge(connectResult.edgeToRemoveId);
            }
        }
        if (connectResult.createEdge) {
            if (m_commands != nullptr) {
                auto createEdgeCommand = std::make_unique<CreateEdgeCommand>(*m_graph, connectResult.edge);
                m_commands->execute(std::move(createEdgeCommand));
            }
            else {
                m_graph->addEdge(connectResult.edge);
            }
        }

        if (!connectResult.handled) {
            std::vector<MoveNodesCommand::MoveItem> moveItems = m_dragController.onMouseUp(*m_graph);

            if (m_gridSettings != nullptr && m_gridSettings->snapEnabled) {
                const float cellSize = std::max(m_gridSettings->cellSize, 1.0f);
                for (MoveNodesCommand::MoveItem &moveItem : moveItems) {
                    const float snappedX = std::round(moveItem.endPosition.x / cellSize) * cellSize;
                    const float snappedY = std::round(moveItem.endPosition.y / cellSize) * cellSize;
                    moveItem.endPosition = { snappedX, snappedY };

                    Node *node = m_graph->findNode(moveItem.nodeId);
                    if (node != nullptr) {
                        node->position = moveItem.endPosition;
                        m_graph->recomputeRoutesForNode(node->id);
                    }
                }

                std::erase_if(moveItems, [](const MoveNodesCommand::MoveItem &moveItem) {
                    return moveItem.startPosition == moveItem.endPosition;
                });
            }

            if (!moveItems.empty() && m_commands != nullptr) {
                auto moveCommand = std::make_unique<MoveNodesCommand>(*m_graph, moveItems);
                m_commands->execute(std::move(moveCommand));
            }

            m_selectionController.onMouseUp(*m_graph, mouseWorld);
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
        m_dragController.update(*m_graph, mouseWorld);
        if (!m_dragController.isDragging()) {
            m_selectionController.onMouseDrag(*m_graph, mouseWorld);
        }
    }

    m_connectController.onMouseMove(mouseWorld);
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

    const bool gridEnabled = m_gridSettings != nullptr ? m_gridSettings->enabled : true;
    const float gridCellSize = m_gridSettings != nullptr ? m_gridSettings->cellSize : 32.0f;
    m_renderer->render(*m_graph, *m_view, vp, gridEnabled, gridCellSize);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFramebuffer);
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::Image(
        (ImTextureID)m_colorTexture,
        canvasSize,
        ImVec2(0.0f, 1.0f),
        ImVec2(1.0f, 0.0f));

    ImGui::End();
}

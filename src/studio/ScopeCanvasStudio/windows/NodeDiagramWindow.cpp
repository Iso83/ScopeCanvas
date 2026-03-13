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
        const Edge *hoveredEdge = EdgeInteractionController::hitTestEdge(graph, mouseWorld, zoom);
        m_hoveredEdgeId = hoveredEdge != nullptr ? hoveredEdge->id : 0;

        const Connector *hoveredConnector =
            EdgeInteractionController::hitTestConnector(graph, mouseWorld, zoom, nullptr);
        m_hoveredConnectorId = hoveredConnector != nullptr ? hoveredConnector->id : 0;
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

    m_renderer->render(graph, *m_view, vp, gridSettings.enabled, gridSettings.cellSize);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFramebuffer);
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::Image(
        (ImTextureID)m_colorTexture,
        canvasSize,
        ImVec2(0.0f, 1.0f),
        ImVec2(1.0f, 0.0f));

    // basic labels in studio layer (engine stays renderer-agnostic regarding fonts/widgets)
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    for (const Node &node : graph.nodes()) {
        const glm::vec4 p = m_renderer->camera().viewProjection() * glm::vec4(node.position.x + 8.0f, node.position.y + 18.0f, 0.0f, 1.0f);
        if (p.w == 0.0f) {
            continue;
        }
        const float ndcX = p.x / p.w;
        const float ndcY = p.y / p.w;
        const float sx = canvasPos.x + ((ndcX + 1.0f) * 0.5f) * canvasSize.x;
        const float sy = canvasPos.y + ((1.0f - ndcY) * 0.5f) * canvasSize.y;
        drawList->AddText(ImVec2(sx, sy), IM_COL32(235, 235, 235, 255), node.title.c_str());
    }

    ImGui::End();
}

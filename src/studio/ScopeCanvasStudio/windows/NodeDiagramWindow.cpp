#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "NodeDiagramWindow.h"

#include <glm/vec4.hpp>

#include <algorithm>
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
    GraphView *view,
    std::string windowTitle)
    : m_window(window),
    m_renderer(renderer),
    m_graph(graph),
    m_view(view),
    m_windowTitle(std::move(windowTitle)) {
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

    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvasSize.x = std::max(canvasSize.x, 1.0f);
    canvasSize.y = std::max(canvasSize.y, 1.0f);

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
    m_renderer->resize(static_cast<int>(canvasSize.x), static_cast<int>(canvasSize.y));

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
            m_graph->removeEdge(connectResult.edgeToRemoveId);
        }
        if (connectResult.createEdge) {
            m_graph->addEdge(connectResult.edge);
        }

        if (!connectResult.handled) {
            m_dragController.onMouseUp(*m_graph);
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

    int fbWidth = 0;
    int fbHeight = 0;
    glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);

    int vx = static_cast<int>(canvasPos.x);
    int vy = static_cast<int>(fbHeight - canvasPos.y - canvasSize.y);
    int vw = static_cast<int>(canvasSize.x);
    int vh = static_cast<int>(canvasSize.y);

    GLint oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, oldViewport);

    glViewport(vx, vy, vw, vh);

    glEnable(GL_SCISSOR_TEST);
    glScissor(vx, vy, vw, vh);

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

    m_renderer->render(*m_graph, *m_view, vp, true, 32.0f);

    glDisable(GL_SCISSOR_TEST);

    glViewport(
        oldViewport[0],
        oldViewport[1],
        oldViewport[2],
        oldViewport[3]);

    ImGui::End();
}

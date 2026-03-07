#include "App.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec4.hpp>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <unordered_set>

namespace {
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app != nullptr) {
        app->onFramebufferSizeChanged(width, height);
    }
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
    auto* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app != nullptr) {
        app->onCursorPos(xPos, yPos);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app != nullptr) {
        app->onMouseButton(button, action, mods);
    }
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    auto* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app != nullptr) {
        app->onScroll(xOffset, yOffset);
    }
}
}

App::App()
    : m_window(nullptr),
      m_renderer(),
      m_model(),
      m_hoveredEdgeId(0),
      m_selectedEdgeId(0),
      m_hoveredConnectorId(0),
      m_draggingEdgeEndpoint(),
      m_deleteHandled(false),
      m_createHandled(false),
      m_duplicateHandled(false),
      m_copyHandled(false),
      m_pasteHandled(false),
      m_undoHandled(false),
      m_redoHandled(false),
      m_initialized(false) {}

App::~App() {
    shutdown();
}

bool App::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(1280, 720, "NodeCanvasGL", nullptr, nullptr);
    if (m_window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD\n";
        shutdown();
        return false;
    }

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetScrollCallback(m_window, scrollCallback);

    if (!m_renderer.init(
        "../assets/shaders/grid.vert",
        "../assets/shaders/grid.frag",
        "../assets/shaders/node.vert",
        "../assets/shaders/node.frag",
        "../assets/shaders/edge.vert",
        "../assets/shaders/edge.frag",
        "../assets/shaders/selection_rect.vert",
        "../assets/shaders/selection_rect.frag",
        width,
        height)) {
        std::cerr << "Failed to initialize renderer\n";
        shutdown();
        return false;
    }

    Node* node1 = m_model.createNode({-280.0f, -40.0f}, {180.0f, 100.0f});
    Node* node2 = m_model.createNode({0.0f, 90.0f}, {220.0f, 120.0f});
    Node* node3 = m_model.createNode({260.0f, -150.0f}, {200.0f, 110.0f});
    (void)node1;
    (void)node3;
    if (node2 != nullptr) {
        node2->selected = true;
    }

    m_model.syncIdCounters();

    m_initialized = true;
    return true;
}

void App::run() {
    if (!m_initialized) {
        return;
    }

    float lastTime = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(m_window)) {
        const float currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        processInput(deltaTime);
        m_renderer.render(m_model,
                          m_hoveredEdgeId,
                          m_hoveredConnectorId,
                          m_selectionController.isBoxSelecting(),
                          m_selectionController.boxStart(),
                          m_selectionController.boxEnd(),
                          m_connectController.isConnecting(),
                          m_connectController.startNodeId(),
                          m_connectController.startConnectorId(),
                          m_connectController.previewPosition());

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void App::shutdown() {
    if (m_window != nullptr) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();
    m_initialized = false;
}

void App::processInput(float deltaTime) {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }

    constexpr float panSpeed = 500.0f;
    glm::vec2 panDelta(0.0f);

    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
        panDelta.x -= panSpeed * deltaTime;
    }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
        panDelta.x += panSpeed * deltaTime;
    }
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
        panDelta.y += panSpeed * deltaTime;
    }
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
        panDelta.y -= panSpeed * deltaTime;
    }

    m_renderer.camera().move(panDelta);

    static float cameraZoom = 1.0f;
    if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS) {
        cameraZoom *= 1.0f + deltaTime;
        m_renderer.camera().setZoom(cameraZoom);
    }
    if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS) {
        cameraZoom *= 1.0f - deltaTime;
        m_renderer.camera().setZoom(cameraZoom);
    }

    const glm::vec2 mouseWorld = screenToWorld(m_input.mouseX, m_input.mouseY);
    const float zoom = m_renderer.camera().zoom();

    const Edge* hoveredEdge = EdgeInteractionController::hitTestEdge(m_model, mouseWorld, zoom);
    m_hoveredEdgeId = hoveredEdge != nullptr ? hoveredEdge->id : 0;

    const Connector* hoveredConnector =
        EdgeInteractionController::hitTestConnector(m_model, mouseWorld, zoom, nullptr);
    m_hoveredConnectorId = hoveredConnector != nullptr ? hoveredConnector->id : 0;

    if (m_input.leftDown && !m_connectController.isConnecting()) {
        m_dragController.update(m_model, mouseWorld);
        if (!m_dragController.isDragging()) {
            m_selectionController.onMouseDrag(m_model, mouseWorld);
        }
    }

    if (m_dragController.isDragging()) {
        m_hoveredEdgeId = 0;
    }

    if (glfwGetKey(m_window, GLFW_KEY_N) == GLFW_PRESS) {
        if (!m_createHandled) {
            const glm::vec2 topLeftWorld = screenToWorld(0.0, 0.0);
            const glm::vec2 bottomRightWorld =
                screenToWorld(static_cast<double>(m_renderer.viewportWidth()),
                              static_cast<double>(m_renderer.viewportHeight()));
            const bool insideView =
                mouseWorld.x >= topLeftWorld.x &&
                mouseWorld.x <= bottomRightWorld.x &&
                mouseWorld.y <= topLeftWorld.y &&
                mouseWorld.y >= bottomRightWorld.y;

            if (insideView) {
                m_model.clearNodeSelection();
                const size_t nodeCountBefore = m_model.nodes().size();
                auto command = std::make_unique<CreateNodeCommand>(m_model, mouseWorld, glm::vec2(200.0f, 120.0f));
                m_commandManager.execute(std::move(command));
                if (m_model.nodes().size() > nodeCountBefore) {
                    Node* createdNode = &m_model.nodes().back();
                    createdNode->selected = true;
                }
                clearEdgeSelection();
            }
        }
        m_createHandled = true;
    } else {
        m_createHandled = false;
    }

    const bool ctrlDown =
        glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(m_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
    if (ctrlDown && glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
        if (!m_duplicateHandled) {
            const Node* selectedNode = nullptr;
            for (const Node& node : m_model.nodes()) {
                if (node.selected) {
                    selectedNode = &node;
                    break;
                }
            }

            if (selectedNode != nullptr) {
                const uint32_t selectedNodeId = selectedNode->id;
                m_model.clearNodeSelection();
                Node* duplicate = m_model.duplicateNode(selectedNodeId);
                if (duplicate != nullptr) {
                    duplicate->selected = true;
                }
                clearEdgeSelection();
            }
        }
        m_duplicateHandled = true;
    } else {
        m_duplicateHandled = false;
    }

    if (ctrlDown && glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!m_copyHandled) {
            copySelectionToClipboard();
        }
        m_copyHandled = true;
    } else {
        m_copyHandled = false;
    }

    if (ctrlDown && glfwGetKey(m_window, GLFW_KEY_V) == GLFW_PRESS) {
        if (!m_pasteHandled) {
            pasteClipboard();
        }
        m_pasteHandled = true;
    } else {
        m_pasteHandled = false;
    }

    if (glfwGetKey(m_window, GLFW_KEY_DELETE) == GLFW_PRESS) {
        if (!m_deleteHandled) {
            bool deletedAnyNode = false;
            std::vector<uint32_t> selectedNodeIds;
            for (const Node& node : m_model.nodes()) {
                if (node.selected) {
                    selectedNodeIds.push_back(node.id);
                }
            }

            for (uint32_t nodeId : selectedNodeIds) {
                auto command = std::make_unique<DeleteNodeCommand>(m_model, nodeId);
                m_commandManager.execute(std::move(command));
                deletedAnyNode = true;
            }

            if (deletedAnyNode) {
                clearEdgeSelection();
                m_hoveredEdgeId = 0;
            } else if (m_selectedEdgeId != 0 && m_model.findEdge(m_selectedEdgeId) != nullptr) {
                auto command = std::make_unique<DeleteEdgeCommand>(m_model, m_selectedEdgeId);
                m_commandManager.execute(std::move(command));
                clearEdgeSelection();
                m_hoveredEdgeId = 0;
            }
        }
        m_deleteHandled = true;
    } else {
        m_deleteHandled = false;
    }

    if (ctrlDown && glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS) {
        if (!m_undoHandled) {
            if (m_commandManager.undo()) {
                clearEdgeSelection();
                m_hoveredEdgeId = 0;
            }
        }
        m_undoHandled = true;
    } else {
        m_undoHandled = false;
    }

    if (ctrlDown && glfwGetKey(m_window, GLFW_KEY_Y) == GLFW_PRESS) {
        if (!m_redoHandled) {
            if (m_commandManager.redo()) {
                clearEdgeSelection();
                m_hoveredEdgeId = 0;
            }
        }
        m_redoHandled = true;
    } else {
        m_redoHandled = false;
    }

    m_connectController.onMouseMove(mouseWorld);

    m_cameraController.update(m_renderer.camera(), m_input);
    m_input.scrollDelta = 0.0f;
}

glm::vec2 App::screenToWorld(double mouseX, double mouseY) {
    const float width = static_cast<float>(m_renderer.viewportWidth());
    const float height = static_cast<float>(m_renderer.viewportHeight());

    const float ndcX = (static_cast<float>(mouseX) / width) * 2.0f - 1.0f;
    const float ndcY = 1.0f - (static_cast<float>(mouseY) / height) * 2.0f;

    const glm::mat4 invViewProjection = m_renderer.camera().invViewProjection();
    const glm::vec4 world = invViewProjection * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
    return glm::vec2(world) / world.w;
}

void App::onFramebufferSizeChanged(int width, int height) {
    m_renderer.resize(width, height);
}

void App::onCursorPos(double xPos, double yPos) {
    m_input.mouseX = xPos;
    m_input.mouseY = yPos;
}

void App::onMouseButton(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            m_input.leftDown = true;

            const glm::vec2 mouseWorld = screenToWorld(m_input.mouseX, m_input.mouseY);
            const float zoom = m_renderer.camera().zoom();
            const bool shiftDown = (mods & GLFW_MOD_SHIFT) != 0;

            const EdgeEndpointHit endpointHit =
                EdgeInteractionController::hitTestEdgeEndpoint(m_model, mouseWorld, zoom);
            if (endpointHit.hit) {
                Edge* edge = nullptr;
                for (Edge& candidate : m_model.edges()) {
                    if (candidate.id == endpointHit.edgeId) {
                        edge = &candidate;
                        break;
                    }
                }

                if (edge != nullptr) {
                    const uint32_t fixedNodeId = endpointHit.startEndpoint ? edge->toNode : edge->fromNode;
                    const uint32_t fixedConnectorId =
                        endpointHit.startEndpoint ? edge->toConnector : edge->fromConnector;

                    auto deleteCommand = std::make_unique<DeleteEdgeCommand>(m_model, edge->id);
                    m_commandManager.execute(std::move(deleteCommand));
                    m_connectController.beginReconnect(
                        endpointHit.edgeId,
                        endpointHit.startEndpoint,
                        fixedNodeId,
                        fixedConnectorId,
                        mouseWorld);
                    clearEdgeSelection();
                    m_draggingEdgeEndpoint = {true, endpointHit.edgeId, endpointHit.startEndpoint};
                    return;
                }
            }

            const bool altDown = (mods & GLFW_MOD_ALT) != 0;
            const bool startConnect = m_connectController.onMouseDown(m_model, mouseWorld, zoom, altDown);
            if (!startConnect) {
                Edge* hitEdge = EdgeInteractionController::hitTestEdge(m_model, mouseWorld, zoom);
                if (hitEdge != nullptr) {
                    for (Node& node : m_model.nodes()) {
                        node.selected = false;
                    }
                    selectEdge(hitEdge->id);
                } else {
                    clearEdgeSelection();
                    m_selectionController.onMouseDown(m_model, mouseWorld, shiftDown);
                }

                const bool wasDragging = m_dragController.isDragging();
                m_dragController.onMouseDown(m_model, mouseWorld);
                if (!wasDragging && m_dragController.isDragging()) {
                    m_hoveredEdgeId = 0;
                }
            } else {
                clearEdgeSelection();
            }
        } else if (action == GLFW_RELEASE) {
            m_input.leftDown = false;

            const glm::vec2 mouseWorld = screenToWorld(m_input.mouseX, m_input.mouseY);
            const ConnectController::ConnectionResult connectResult =
                m_connectController.onMouseUp(m_model, mouseWorld, m_renderer.camera().zoom());
            if (connectResult.createEdge) {
                auto createEdgeCommand = std::make_unique<CreateEdgeCommand>(m_model, connectResult.edge);
                m_commandManager.execute(std::move(createEdgeCommand));
            }

            if (!connectResult.handled) {
                const std::vector<MoveNodesCommand::MoveItem> moveItems = m_dragController.onMouseUp(m_model);
                if (!moveItems.empty()) {
                    auto moveCommand = std::make_unique<MoveNodesCommand>(m_model, moveItems);
                    m_commandManager.execute(std::move(moveCommand));
                }
                m_selectionController.onMouseUp(m_model, mouseWorld);
            }

            if (m_draggingEdgeEndpoint.active) {
                m_draggingEdgeEndpoint = {};
            }
        }
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            m_input.middleDown = true;
        } else if (action == GLFW_RELEASE) {
            m_input.middleDown = false;
        }
    }
}

void App::onScroll(double xOffset, double yOffset) {
    (void)xOffset;
    m_input.scrollDelta += static_cast<float>(yOffset);
}

void App::clearEdgeSelection() {
    m_selectedEdgeId = 0;
    for (Edge& edge : m_model.edges()) {
        edge.selected = false;
    }
}

void App::selectEdge(uint32_t edgeId) {
    m_selectedEdgeId = edgeId;
    for (Edge& edge : m_model.edges()) {
        edge.selected = (edge.id == edgeId);
    }
}

void App::copySelectionToClipboard() {
    m_clipboard = {};

    std::unordered_map<uint32_t, const Node*> selectedNodes;
    glm::vec2 minPosition(0.0f);
    bool hasMin = false;
    for (const Node& node : m_model.nodes()) {
        if (!node.selected) {
            continue;
        }

        selectedNodes[node.id] = &node;
        if (!hasMin) {
            minPosition = node.position;
            hasMin = true;
        } else {
            minPosition.x = std::min(minPosition.x, node.position.x);
            minPosition.y = std::min(minPosition.y, node.position.y);
        }
    }

    if (selectedNodes.empty()) {
        return;
    }

    m_clipboard.origin = minPosition;

    for (const auto& [nodeId, node] : selectedNodes) {
        ClipboardNode clipNode;
        clipNode.originalNodeId = nodeId;
        clipNode.relativePosition = node->position - minPosition;
        clipNode.size = node->size;
        clipNode.connectors.reserve(node->connectors.size());
        for (const Connector& connector : node->connectors) {
            clipNode.connectors.push_back({connector.side, connector.offset});
        }
        m_clipboard.nodes.push_back(clipNode);
    }

    const std::unordered_set<uint32_t> selectedNodeIds = [&selectedNodes]() {
        std::unordered_set<uint32_t> ids;
        for (const auto& [nodeId, node] : selectedNodes) {
            (void)node;
            ids.insert(nodeId);
        }
        return ids;
    }();

    for (const Edge& edge : m_model.edges()) {
        if (selectedNodeIds.contains(edge.fromNode) && selectedNodeIds.contains(edge.toNode)) {
            m_clipboard.edges.push_back({edge.fromNode, edge.fromConnector, edge.toNode, edge.toConnector});
        }
    }
}

void App::pasteClipboard() {
    if (m_clipboard.empty()) {
        return;
    }

    std::unordered_map<uint32_t, uint32_t> nodeIdMap;
    std::unordered_map<uint32_t, uint32_t> connectorIdMap;

    m_model.clearNodeSelection();

    constexpr glm::vec2 pasteOffset(40.0f, 40.0f);
    for (const ClipboardNode& clipboardNode : m_clipboard.nodes) {
        Node* newNode = m_model.createNodeWithConnectors(
            m_clipboard.origin + clipboardNode.relativePosition + pasteOffset,
            clipboardNode.size,
            clipboardNode.connectors);

        if (newNode == nullptr) {
            continue;
        }

        newNode->selected = true;
        nodeIdMap[clipboardNode.originalNodeId] = newNode->id;

        const Node* sourceNode = m_model.findNode(clipboardNode.originalNodeId);
        if (sourceNode == nullptr) {
            continue;
        }

        const size_t connectorCount = std::min(sourceNode->connectors.size(), newNode->connectors.size());
        for (size_t i = 0; i < connectorCount; ++i) {
            connectorIdMap[sourceNode->connectors[i].id] = newNode->connectors[i].id;
        }
    }

    for (const ClipboardEdge& clipboardEdge : m_clipboard.edges) {
        const auto fromNodeIt = nodeIdMap.find(clipboardEdge.fromNode);
        const auto toNodeIt = nodeIdMap.find(clipboardEdge.toNode);
        const auto fromConnectorIt = connectorIdMap.find(clipboardEdge.fromConnector);
        const auto toConnectorIt = connectorIdMap.find(clipboardEdge.toConnector);
        if (fromNodeIt == nodeIdMap.end() ||
            toNodeIt == nodeIdMap.end() ||
            fromConnectorIt == connectorIdMap.end() ||
            toConnectorIt == connectorIdMap.end()) {
            continue;
        }

        m_model.createEdge(fromNodeIt->second,
                           fromConnectorIt->second,
                           toNodeIt->second,
                           toConnectorIt->second);
    }

    clearEdgeSelection();
}

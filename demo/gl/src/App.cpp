#include "App.h"

#include <ScopeCanvas/widget/theme/NodeVisualRegistry.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "GLFWTrampoline.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <glm/geometric.hpp>
#include <glm/vec4.hpp>
#include <iostream>

using namespace sc::glfw;

namespace {
void setupNode(ScopeCanvas::Core::GraphDocument& document, ScopeCanvas::Core::CanvasNodeId id,
               const glm::vec2& position, const glm::vec2& size) {
    if (ScopeCanvas::Core::Node* node = document.getNode(id); node != nullptr) {
        node->setPosition(position);
        node->setSize(size);
    }
}

bool pointInNode(const glm::vec2& point, const ScopeCanvas::Core::Node& node) {
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

bool canConnect(const ScopeCanvas::Core::GraphDocument& model, ScopeCanvas::Core::CanvasConnectorId a,
                ScopeCanvas::Core::CanvasConnectorId b) {
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

ScopeCanvas::Render::Renderers::NodeRenderStyle toRenderStyle(const ScopeCanvas::Render::Theme::NodeVisual& visual) {
    auto rgba = [](const ScopeCanvas::Render::Theme::ColorRgba8& color) {
        return glm::vec4(static_cast<float>(color.r) / 255.0F, static_cast<float>(color.g) / 255.0F,
                         static_cast<float>(color.b) / 255.0F, static_cast<float>(color.a) / 255.0F);
    };

    ScopeCanvas::Render::Renderers::NodeRenderStyle style{};
    style.bodyColor = rgba(visual.bodyColor);
    style.headerColor = rgba(visual.titleBarColor);
    style.borderColor = rgba(visual.borderColor);
    style.selectionColor = rgba(visual.selectionColor);
    style.textColor = rgba(visual.titleTextColor);
    style.borderThickness = visual.borderThickness;
    style.headerHeight = visual.titleBarHeight;
    return style;
}
} // namespace

App::App() = default;

App::~App() {
    shutdown();
}

bool App::init() {
    if (!initWindow()) {
        return false;
    }
    if (!initGL()) {
        shutdown();
        return false;
    }

    setupCallbacks();
    initializeDocument();

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    m_camera.setViewportSize(width, height);
    m_camera.setPosition({120.0F, 0.0F});
    m_camera.setZoom(1.0F);

    if (!m_renderer.init()) {
        std::cerr << "Failed to initialize CanvasRenderer\n";
        shutdown();
        return false;
    }

    m_renderOptions.nodeStyleResolver = [](ScopeCanvas::Core::NodeTypeId typeId) {
        static const ScopeCanvas::Render::Theme::NodeVisualRegistry registry{};
        return toRenderStyle(registry.getVisual(typeId));
    };
    m_renderOptions.nodeTitleResolver = [](ScopeCanvas::Core::NodeTypeId typeId) {
        static const ScopeCanvas::Render::Theme::NodeVisualRegistry registry{};
        return registry.getVisual(typeId).title;
    };
    m_renderOptions.gridSize = 32.0F;

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

        m_renderOptions.selectedNodeIds = m_selectedNodes;
        m_renderOptions.selectionRectActive = m_selectionRectActive;
        m_renderOptions.selectionRectStart = m_selectionRectStart;
        m_renderOptions.selectionRectEnd = m_selectionRectEnd;
        m_renderOptions.hoveredConnectorId = pickConnector(screenToWorld(m_input.mouseX, m_input.mouseY));
        m_renderOptions.activeConnectorId = m_activeConnector;
        m_renderOptions.previewEdgeActive = m_activeConnector.isValid();
        m_renderOptions.previewEdgeEnd = m_previewEdgeEnd;
        if (m_activeConnector.isValid()) {
            if (const ScopeCanvas::Core::Connector* connector = m_document.getConnector(m_activeConnector);
                connector != nullptr) {
                if (const ScopeCanvas::Core::Node* node = m_document.getNode(connector->nodeId); node != nullptr) {
                    for (std::size_t i = 0; i < node->connectors.size(); ++i) {
                        if (node->connectors[i] == m_activeConnector) {
                            m_renderOptions.previewEdgeStart = connectorWorld(*node, i);
                            break;
                        }
                    }
                }
            }
        }

        const auto routes = routeAllEdges();
        m_renderOptions.hoveredEdgeId = pickEdge(screenToWorld(m_input.mouseX, m_input.mouseY), routes);
        m_renderer.render(m_document, routes, m_camera, m_renderOptions);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

bool App::initWindow() {
    if (!glfwInit()) {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(1280, 720, "ScopeCanvas DemoGL", nullptr, nullptr);
    if (m_window == nullptr) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    return true;
}

bool App::initGL() {
    return gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) != 0;
}

void App::initializeDocument() {
    using namespace ScopeCanvas;

    const auto a = m_document.createNode(Core::NodeTypeId{10});
    const auto b = m_document.createNode(Core::NodeTypeId{11});
    const auto c = m_document.createNode(Core::NodeTypeId{15});
    const auto d = m_document.createNode(Core::NodeTypeId{4});

    setupNode(m_document, a, {-288.0F, -96.0F}, {224.0F, 128.0F});
    setupNode(m_document, b, {32.0F, -160.0F}, {188.0F, 104.0F});
    setupNode(m_document, c, {48.0F, 80.0F}, {220.0F, 112.0F});
    setupNode(m_document, d, {344.0F, -16.0F}, {196.0F, 112.0F});

    const Core::Node* n0 = m_document.getNode(a);
    const Core::Node* n1 = m_document.getNode(b);
    const Core::Node* n2 = m_document.getNode(c);
    const Core::Node* n3 = m_document.getNode(d);
    if (n0 != nullptr && n1 != nullptr && n2 != nullptr && n3 != nullptr) {
        m_document.connect(n0->connectors[1], n1->connectors[0]);
        m_document.connect(n0->connectors[1], n2->connectors[0]);
        m_document.connect(n2->connectors[1], n3->connectors[0]);
    }

    m_nodeIds = {a, b, c, d};
}

void App::shutdown() {
    if (m_initialized) {
        m_renderer.shutdown();
    }

    if (m_window != nullptr) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();
    m_initialized = false;
}

glm::vec2 App::screenToWorld(double x, double y) const {
    int width = 1;
    int height = 1;
    glfwGetFramebufferSize(m_window, &width, &height);
    const float nx = static_cast<float>(x) / static_cast<float>(width) * 2.0F - 1.0F;
    const float ny = 1.0F - static_cast<float>(y) / static_cast<float>(height) * 2.0F;
    const glm::mat4 inv = m_camera.invViewProjection();
    const glm::vec4 world = inv * glm::vec4(nx, ny, 0.0F, 1.0F);
    return {world.x / world.w, world.y / world.w};
}

glm::vec2 App::connectorWorld(const ScopeCanvas::Core::Node& node, std::size_t index) const {
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


std::vector<ScopeCanvas::Routing::EdgeRoute> App::routeAllEdges() const {
    std::vector<ScopeCanvas::Routing::EdgeRoute> routes = m_router.routeAll(m_document);
    if (!routes.empty()) {
        return routes;
    }

    constexpr std::uint32_t kMaxConsecutiveMissingEdgeIds = 128;
    std::uint32_t missStreak = 0;
    for (std::uint32_t probe = 1; missStreak < kMaxConsecutiveMissingEdgeIds; ++probe) {
        const ScopeCanvas::Core::Edge* edge = m_document.getEdge(ScopeCanvas::Core::CanvasEdgeId{probe});
        if (edge == nullptr) {
            ++missStreak;
            continue;
        }
        missStreak = 0;
        const ScopeCanvas::Core::Connector* from = m_document.getConnector(edge->fromConnector);
        const ScopeCanvas::Core::Connector* to = m_document.getConnector(edge->toConnector);
        if (from == nullptr || to == nullptr) {
            continue;
        }
        const ScopeCanvas::Core::Node* fromNode = m_document.getNode(from->nodeId);
        const ScopeCanvas::Core::Node* toNode = m_document.getNode(to->nodeId);
        if (fromNode == nullptr || toNode == nullptr) {
            continue;
        }
        std::size_t fromIndex = 0;
        while (fromIndex < fromNode->connectors.size() && fromNode->connectors[fromIndex] != edge->fromConnector) {
            ++fromIndex;
        }
        std::size_t toIndex = 0;
        while (toIndex < toNode->connectors.size() && toNode->connectors[toIndex] != edge->toConnector) {
            ++toIndex;
        }
        ScopeCanvas::Routing::EdgeRoute route{};
        route.edgeId = edge->id;
        route.points.push_back(connectorWorld(*fromNode, std::min(fromIndex, fromNode->connectors.size() - 1U)));
        route.points.push_back(connectorWorld(*toNode, std::min(toIndex, toNode->connectors.size() - 1U)));
        routes.push_back(route);
    }
    return routes;
}

ScopeCanvas::Core::CanvasEdgeId App::pickEdge(const glm::vec2& world, const std::vector<ScopeCanvas::Routing::EdgeRoute>& routes) const {
    const float thresholdSquared = 100.0F / (m_camera.zoom() * m_camera.zoom());
    for (const auto& route : routes) {
        for (std::size_t i = 0; i + 1 < route.points.size(); ++i) {
            if (distanceToSegmentSquared(world, route.points[i], route.points[i + 1]) <= thresholdSquared) {
                return route.edgeId;
            }
        }
    }
    return {};
}

ScopeCanvas::Core::CanvasNodeId App::pickNode(const glm::vec2& world) const {
    for (auto it = m_nodeIds.rbegin(); it != m_nodeIds.rend(); ++it) {
        if (const ScopeCanvas::Core::Node* node = m_document.getNode(*it); node != nullptr && pointInNode(world, *node)) {
            return *it;
        }
    }
    return {};
}

ScopeCanvas::Core::CanvasConnectorId App::pickConnector(const glm::vec2& world) const {
    const float pickRadiusSquared = 100.0F / (m_camera.zoom() * m_camera.zoom());
    for (auto it = m_nodeIds.rbegin(); it != m_nodeIds.rend(); ++it) {
        const ScopeCanvas::Core::Node* node = m_document.getNode(*it);
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

void App::clearSelection() {
    m_selectedNodes.clear();
}

void App::setSingleSelection(ScopeCanvas::Core::CanvasNodeId nodeId) {
    m_selectedNodes.clear();
    if (nodeId.isValid()) {
        m_selectedNodes.push_back(nodeId);
    }
}

bool App::isNodeSelected(ScopeCanvas::Core::CanvasNodeId nodeId) const {
    return std::find(m_selectedNodes.begin(), m_selectedNodes.end(), nodeId) != m_selectedNodes.end();
}

void App::applySelectionRect() {
    const glm::vec2 rectMin(std::min(m_selectionRectStart.x, m_selectionRectEnd.x),
                            std::min(m_selectionRectStart.y, m_selectionRectEnd.y));
    const glm::vec2 rectMax(std::max(m_selectionRectStart.x, m_selectionRectEnd.x),
                            std::max(m_selectionRectStart.y, m_selectionRectEnd.y));

    m_selectedNodes.clear();
    for (ScopeCanvas::Core::CanvasNodeId nodeId : m_nodeIds) {
        const ScopeCanvas::Core::Node* node = m_document.getNode(nodeId);
        if (node == nullptr) {
            continue;
        }
        const glm::vec2 nodeMin = node->position;
        const glm::vec2 nodeMax = node->position + node->size;
        const bool overlapX = nodeMax.x >= rectMin.x && nodeMin.x <= rectMax.x;
        const bool overlapY = nodeMax.y >= rectMin.y && nodeMin.y <= rectMax.y;
        if (overlapX && overlapY) {
            m_selectedNodes.push_back(nodeId);
        }
    }
}

glm::vec2 App::snapToGrid(glm::vec2 position) const {
    if (!m_renderOptions.showGrid || m_renderOptions.gridSize <= 0.0F) {
        return position;
    }
    const float s = m_renderOptions.gridSize;
    position.x = std::round(position.x / s) * s;
    position.y = std::round(position.y / s) * s;
    return position;
}

void App::processInput(float deltaTime) {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
    if (glfwGetKey(m_window, GLFW_KEY_G) == GLFW_PRESS) {
        m_renderOptions.showGrid = true;
    }
    if (glfwGetKey(m_window, GLFW_KEY_F1) == GLFW_PRESS) {
        m_debugEnabled = true;
        m_renderOptions.showDebug = true;
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
    m_camera.move(panDelta);

    if (m_input.scrollDelta != 0.0f) {
        m_camera.setZoom(m_camera.zoom() + m_input.scrollDelta * 0.1F);
        m_input.scrollDelta = 0.0f;
    }

    const glm::vec2 mouseWorld = screenToWorld(m_input.mouseX, m_input.mouseY);
    m_previewEdgeEnd = mouseWorld;
    const bool leftPressed = m_input.leftDown && !m_input.previousLeftDown;
    const bool leftReleased = !m_input.leftDown && m_input.previousLeftDown;

    if (m_input.middleDown && m_input.previousMiddleDown) {
        static double previousMouseX = m_input.mouseX;
        static double previousMouseY = m_input.mouseY;
        const glm::vec2 delta = {static_cast<float>(m_input.mouseX - previousMouseX),
                                 static_cast<float>(m_input.mouseY - previousMouseY)};
        m_camera.move({-delta.x / m_camera.zoom(), delta.y / m_camera.zoom()});
        previousMouseX = m_input.mouseX;
        previousMouseY = m_input.mouseY;
    }

    if (leftPressed) {
        const auto hoveredConnector = pickConnector(mouseWorld);
        if (hoveredConnector.isValid()) {
            m_activeConnector = hoveredConnector;
            m_selectionRectActive = false;
            m_renderOptions.selectedEdgeId = {};
        } else {
            const auto pickedNode = pickNode(mouseWorld);
            if (pickedNode.isValid()) {
                m_dragNode = pickedNode;
                m_renderOptions.selectedEdgeId = {};
                if (!isNodeSelected(pickedNode)) {
                    setSingleSelection(pickedNode);
                }
                m_dragSelection = m_selectedNodes;
                m_dragSelectionStartPositions.clear();
                for (const auto nodeId : m_dragSelection) {
                    if (const ScopeCanvas::Core::Node* node = m_document.getNode(nodeId); node != nullptr) {
                        m_dragSelectionStartPositions.push_back(node->position);
                    }
                }
                if (const ScopeCanvas::Core::Node* node = m_document.getNode(pickedNode); node != nullptr) {
                    m_dragOffset = mouseWorld - node->position;
                    m_dragAnchorStartPosition = node->position;
                }
            } else {
                const ScopeCanvas::Core::CanvasEdgeId pickedEdge = pickEdge(mouseWorld, routeAllEdges());
                m_renderOptions.selectedEdgeId = pickedEdge;
                if (pickedEdge.isValid()) {
                    clearSelection();
                } else {
                    clearSelection();
                    m_selectionRectActive = true;
                    m_selectionRectStart = mouseWorld;
                    m_selectionRectEnd = mouseWorld;
                }
            }
        }
    }

    if (m_input.leftDown) {
        if (m_dragNode.isValid()) {
            const glm::vec2 anchorPosition = snapToGrid(mouseWorld - m_dragOffset);
            if (const ScopeCanvas::Core::Node* draggedNode = m_document.getNode(m_dragNode); draggedNode != nullptr) {
                const glm::vec2 delta = anchorPosition - m_dragAnchorStartPosition;
                for (std::size_t i = 0; i < m_dragSelection.size() && i < m_dragSelectionStartPositions.size(); ++i) {
                    if (ScopeCanvas::Core::Node* node = m_document.getNode(m_dragSelection[i]); node != nullptr) {
                        node->setPosition(snapToGrid(m_dragSelectionStartPositions[i] + delta));
                    }
                }
            }
        } else if (m_selectionRectActive) {
            m_selectionRectEnd = mouseWorld;
            applySelectionRect();
        }
    }

    if (leftReleased) {
        if (m_activeConnector.isValid()) {
            const auto targetConnector = pickConnector(mouseWorld);
            if (targetConnector.isValid() && targetConnector != m_activeConnector &&
                canConnect(m_document, m_activeConnector, targetConnector)) {
                m_document.connect(m_activeConnector, targetConnector);
            }
            m_activeConnector = {};
        } else if (m_selectionRectActive) {
            applySelectionRect();
            m_selectionRectActive = false;
        }
        m_dragNode = {};
        m_dragSelection.clear();
        m_dragSelectionStartPositions.clear();
    }

    m_input.previousLeftDown = m_input.leftDown;
    m_input.previousMiddleDown = m_input.middleDown;
}

void App::setupCallbacks() {
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSize<App, &App::onFramebufferSizeChanged>);
    glfwSetCursorPosCallback(m_window, cursorPos<App, &App::onCursorPos>);
    glfwSetMouseButtonCallback(m_window, mouseButton<App, &App::onMouseButton>);
    glfwSetScrollCallback(m_window, scroll<App, &App::onScroll>);
}

void App::onFramebufferSizeChanged(int width, int height) {
    m_camera.setViewportSize(width, height);
    glViewport(0, 0, width, height);
}

void App::onMouseButton(int button, int action, int mods) {
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        m_input.leftDown = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        m_input.middleDown = (action == GLFW_PRESS);
    }
}

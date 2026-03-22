#include "App.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "GLFWTrampoline.h"

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
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

        m_renderOptions.selectedNodeId = m_selectedNode;
        m_renderOptions.selectionRectActive = m_selectionRectActive;
        m_renderOptions.selectionRectStart = m_selectionRectStart;
        m_renderOptions.selectionRectEnd = m_selectionRectEnd;

        const auto routes = m_router.routeAll(m_document);
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

    setupNode(m_document, a, {-280.0F, -60.0F}, {220.0F, 120.0F});
    setupNode(m_document, b, {20.0F, -140.0F}, {180.0F, 100.0F});
    setupNode(m_document, c, {20.0F, 80.0F}, {220.0F, 110.0F});
    setupNode(m_document, d, {320.0F, -30.0F}, {200.0F, 110.0F});

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
    m_camera.move(panDelta);

    if (m_input.scrollDelta != 0.0f) {
        m_camera.setZoom(m_camera.zoom() + m_input.scrollDelta * 0.1F);
        m_input.scrollDelta = 0.0f;
    }

    const glm::vec2 mouseWorld = screenToWorld(m_input.mouseX, m_input.mouseY);
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
        m_dragNode = {};
        for (auto it = m_nodeIds.rbegin(); it != m_nodeIds.rend(); ++it) {
            if (const ScopeCanvas::Core::Node* node = m_document.getNode(*it);
                node != nullptr && pointInNode(mouseWorld, *node)) {
                m_dragNode = *it;
                m_selectedNode = *it;
                m_dragOffset = mouseWorld - node->position;
                break;
            }
        }

        if (!m_dragNode.isValid()) {
            m_selectedNode = {};
            m_selectionRectActive = true;
            m_selectionRectStart = mouseWorld;
            m_selectionRectEnd = mouseWorld;
        }
    }

    if (m_input.leftDown) {
        if (m_dragNode.isValid()) {
            if (ScopeCanvas::Core::Node* node = m_document.getNode(m_dragNode); node != nullptr) {
                node->setPosition(mouseWorld - m_dragOffset);
            }
        } else if (m_selectionRectActive) {
            m_selectionRectEnd = mouseWorld;
        }
    }

    if (leftReleased) {
        m_dragNode = {};
        m_selectionRectActive = false;
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

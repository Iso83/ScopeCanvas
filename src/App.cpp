#include "App.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <iostream>

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

App::App() : m_window(nullptr), m_renderer(), m_model(), m_initialized(false) {}

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

    if (!m_renderer.init("../assets/shaders/grid.vert",
        "../assets/shaders/grid.frag",
        "../assets/shaders/basic.vert",
        "../assets/shaders/node.vert",
        "../assets/shaders/edge.vert",
        "../assets/shaders/edge.frag",
        width,
        height)) {
        std::cerr << "Failed to initialize renderer\n";
        shutdown();
        return false;
    }

    m_model.nodes() = {
        Node{1, {-280.0f, -40.0f}, {180.0f, 100.0f}, false, {}},
        Node{2, {0.0f, 90.0f}, {220.0f, 120.0f}, true, {}},
        Node{3, {260.0f, -150.0f}, {200.0f, 110.0f}, false, {}},
    };

    uint32_t nextConnectorId = 1;
    for (Node& node : m_model.nodes()) {
        node.connectors = createDefaultConnectors(node.id, nextConnectorId);
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
        m_renderer.render(m_model,
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

    static float zoom = 1.0f;
    if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS) {
        zoom *= 1.0f + deltaTime;
        m_renderer.camera().setZoom(zoom);
    }
    if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS) {
        zoom *= 1.0f - deltaTime;
        m_renderer.camera().setZoom(zoom);
    }

    const glm::vec2 mouseWorld = screenToWorld(m_input.mouseX, m_input.mouseY);
    if (m_input.leftDown && !m_connectController.isConnecting()) {
        m_dragController.update(mouseWorld);
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
    //const glm::mat4 invViewProjection = glm::inverse(m_renderer.camera().viewProjection());
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
    (void)mods;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            m_input.leftDown = true;

            const glm::vec2 mouseWorld = screenToWorld(m_input.mouseX, m_input.mouseY);
            const bool startConnect = m_connectController.onMouseDown(m_model, mouseWorld);
            if (!startConnect) {
                m_selectionController.onMouseDown(m_model, mouseWorld);
                m_dragController.onMouseDown(m_model, mouseWorld);
            }
        } else if (action == GLFW_RELEASE) {
            m_input.leftDown = false;

            const glm::vec2 mouseWorld = screenToWorld(m_input.mouseX, m_input.mouseY);
            const bool wasConnecting = m_connectController.onMouseUp(m_model, mouseWorld);
            if (!wasConnecting) {
                m_dragController.onMouseUp();
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

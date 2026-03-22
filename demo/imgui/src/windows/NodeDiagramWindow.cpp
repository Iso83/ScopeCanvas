#include "windows/NodeDiagramWindow.h"

#include <algorithm>
#include <cmath>
#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <imgui.h>

namespace ScopeCanvas::Studio {
namespace {
glm::vec2 connectorWorld(const Core::Node& node, std::size_t index) {
    const float count = static_cast<float>(node.connectors.size() + 1U);
    const float y = node.position.y + (node.size.y / count) * static_cast<float>(index + 1U);
    const bool right = (index % 2U) == 1U;
    return {right ? node.position.x + node.size.x : node.position.x, y};
}

bool pointInNode(const glm::vec2& point, const Core::Node& node) {
    return point.x >= node.position.x && point.x <= node.position.x + node.size.x && point.y >= node.position.y &&
           point.y <= node.position.y + node.size.y;
}
} // namespace

NodeDiagramWindow::NodeDiagramWindow(GLFWwindow* window, DiagramBasics* basics, ViewState* viewState, std::string title)
    : m_window(window), m_basics(basics), m_viewState(viewState), m_title(std::move(title)) {}

NodeDiagramWindow::~NodeDiagramWindow() {
    releaseRenderTarget();
    if (m_rendererInitialized) {
        m_renderer.shutdown();
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
    }
    if (m_colorTexture != 0) {
        glDeleteTextures(1, &m_colorTexture);
    }
    if (m_framebuffer != 0) {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
    m_depthStencilRenderbuffer = 0;
    m_colorTexture = 0;
    m_framebuffer = 0;
}

glm::vec2 NodeDiagramWindow::screenToWorld(float sx, float sy, float w, float h) const {
    const float nx = (sx / w) * 2.0F - 1.0F;
    const float ny = 1.0F - (sy / h) * 2.0F;
    const glm::mat4 inv = m_camera.invViewProjection();
    const glm::vec4 v = inv * glm::vec4(nx, ny, 0.0F, 1.0F);
    return {v.x / v.w, v.y / v.w};
}

void NodeDiagramWindow::draw() {
    if (m_basics == nullptr || m_viewState == nullptr) {
        return;
    }
    if (!m_rendererInitialized) {
        m_rendererInitialized = m_renderer.init();
    }

    ImGui::Begin(m_title.c_str());
    const ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    ensureRenderTarget(static_cast<int>(canvasSize.x), static_cast<int>(canvasSize.y));

    m_camera.setViewportSize(std::max(1, static_cast<int>(canvasSize.x)), std::max(1, static_cast<int>(canvasSize.y)));
    m_camera.setPosition({m_viewState->cameraX, m_viewState->cameraY});
    m_camera.setZoom(m_viewState->zoom);

    GLint oldFb = 0;
    GLint oldVp[4]{};
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
    glGetIntegerv(GL_VIEWPORT, oldVp);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_renderWidth, m_renderHeight);

    Render::CanvasRenderOptions options{};
    options.showGrid = m_basics->gridSettings().enabled;
    options.gridSize = m_basics->gridSettings().cellSize;
    options.selectedNodeId = m_selectedNode;
    options.selectionRectActive = m_selectionRectActive;
    options.selectionRectStart = m_selectionRectStart;
    options.selectionRectEnd = m_selectionRectEnd;
    m_renderer.render(m_basics->model(), m_basics->routeAllEdges(), m_camera, options);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFb);
    glViewport(oldVp[0], oldVp[1], oldVp[2], oldVp[3]);

    ImGui::Image((ImTextureID)(intptr_t)m_colorTexture, canvasSize, ImVec2(0, 1), ImVec2(1, 0));

    const bool hovered = ImGui::IsItemHovered();
    const bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    const bool dragging = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
    const bool released = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    const ImVec2 mouse = ImGui::GetIO().MousePos;
    const glm::vec2 mouseWorld =
        screenToWorld(mouse.x - canvasPos.x, mouse.y - canvasPos.y, canvasSize.x, canvasSize.y);

    if (hovered && ImGui::GetIO().MouseWheel != 0.0F) {
        m_viewState->zoom = std::max(0.05F, m_viewState->zoom + ImGui::GetIO().MouseWheel * 0.1F);
    }
    if (hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        const ImVec2 delta = ImGui::GetIO().MouseDelta;
        m_viewState->cameraX -= delta.x / m_viewState->zoom;
        m_viewState->cameraY += delta.y / m_viewState->zoom;
    }

    if (clicked) {
        m_dragNode = {};
        m_selectedNode = {};
        m_selectionRectActive = false;

        for (Core::CanvasNodeId nodeId : m_basics->nodeIds()) {
            const Core::Node* node = m_basics->model().getNode(nodeId);
            if (node == nullptr) {
                continue;
            }

            for (std::size_t i = 0; i < node->connectors.size(); ++i) {
                const glm::vec2 cw = connectorWorld(*node, i);
                const glm::vec2 delta = mouseWorld - cw;
                if (glm::dot(delta, delta) <= 100.0F / (m_viewState->zoom * m_viewState->zoom)) {
                    const Core::CanvasConnectorId picked = node->connectors[i];
                    if (!m_pendingConnector.isValid()) {
                        m_pendingConnector = picked;
                    } else if (m_pendingConnector != picked) {
                        (void)m_basics->connect(m_pendingConnector, picked);
                        m_pendingConnector = {};
                    }
                    ImGui::End();
                    return;
                }
            }

            if (pointInNode(mouseWorld, *node)) {
                m_dragNode = nodeId;
                m_selectedNode = nodeId;
                m_dragOffset = mouseWorld - node->position;
                break;
            }
        }

        if (!m_dragNode.isValid()) {
            m_selectionRectActive = true;
            m_selectionRectStart = mouseWorld;
            m_selectionRectEnd = mouseWorld;
        }
    }

    if (dragging && m_dragNode.isValid()) {
        if (Core::Node* node = m_basics->model().getNode(m_dragNode); node != nullptr) {
            glm::vec2 position = mouseWorld - m_dragOffset;
            if (m_basics->gridSettings().snapEnabled) {
                const float s = m_basics->gridSettings().cellSize;
                position.x = std::round(position.x / s) * s;
                position.y = std::round(position.y / s) * s;
            }
            node->setPosition(position);
        }
    } else if (dragging && m_selectionRectActive) {
        m_selectionRectEnd = mouseWorld;
    }
    if (released) {
        m_dragNode = {};
        m_selectionRectActive = false;
    }

    if (hovered && ImGui::IsKeyPressed(ImGuiKey_Delete) && m_selectedNode.isValid()) {
        m_basics->deleteNode(m_selectedNode);
        m_selectedNode = {};
        m_dragNode = {};
    }

    ImGui::End();
}
} // namespace ScopeCanvas::Studio

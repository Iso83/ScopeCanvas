#include "windows/NodeDiagramWindow.h"

#include "Renderers/CanvasRenderer.h"
#include "Scene/SceneBuilder.h"
#include "Theme/NodeVisualRegistry.h"

#include <glad/glad.h>
#include <imgui.h>

#include <algorithm>

namespace ScopeCanvas::Studio
{
namespace
{
ImU32 color(const Render::Theme::ColorRgba8& c) { return IM_COL32(c.r, c.g, c.b, c.a); }

Engine::Core::Vec2 connectorWorld(const Engine::Core::Node& node, std::size_t index)
{
    const float count = static_cast<float>(node.connectors.size() + 1U);
    const float y = node.position.y + (node.size.y / count) * static_cast<float>(index + 1U);
    const bool right = index % 2U == 1U;
    return {right ? node.position.x + node.size.x : node.position.x, y};
}

Engine::Core::NodeTypeId mapType(const Engine::Core::Node& node)
{
    return node.typeId.isValid() ? node.typeId : Engine::Core::NodeTypeId{1};
}
} // namespace

NodeDiagramWindow::NodeDiagramWindow(GLFWwindow* window, DiagramBasics* basics, ViewState* viewState, std::string title)
    : m_window(window), m_basics(basics), m_viewState(viewState), m_title(std::move(title))
{
}

NodeDiagramWindow::~NodeDiagramWindow() { releaseRenderTarget(); }

void NodeDiagramWindow::ensureRenderTarget(int width, int height)
{
    width = std::max(width, 1);
    height = std::max(height, 1);
    if (m_framebuffer != 0 && m_renderWidth == width && m_renderHeight == height)
    {
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

void NodeDiagramWindow::releaseRenderTarget()
{
    if (m_depthStencilRenderbuffer != 0) glDeleteRenderbuffers(1, &m_depthStencilRenderbuffer);
    if (m_colorTexture != 0) glDeleteTextures(1, &m_colorTexture);
    if (m_framebuffer != 0) glDeleteFramebuffers(1, &m_framebuffer);
    m_depthStencilRenderbuffer = 0;
    m_colorTexture = 0;
    m_framebuffer = 0;
}

Engine::Core::Vec2 NodeDiagramWindow::screenToWorld(float sx, float sy, float w, float h) const
{
    const float nx = (sx / w) * 2.0F - 1.0F;
    const float ny = 1.0F - (sy / h) * 2.0F;
    const glm::mat4 inv = m_camera.invViewProjection();
    const glm::vec4 v = inv * glm::vec4(nx, ny, 0.0F, 1.0F);
    return {v.x / v.w, v.y / v.w};
}

void NodeDiagramWindow::draw()
{
    if (m_basics == nullptr || m_viewState == nullptr) return;

    ImGui::Begin(m_title.c_str());
    const ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    ensureRenderTarget(static_cast<int>(canvasSize.x), static_cast<int>(canvasSize.y));

    m_camera.setViewportSize(std::max(1, static_cast<int>(canvasSize.x)), std::max(1, static_cast<int>(canvasSize.y)));
    m_camera.setPosition({m_viewState->cameraX, m_viewState->cameraY});
    m_camera.setZoom(m_viewState->zoom);

    GLint oldFb = 0;
    GLint oldVp[4];
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
    glGetIntegerv(GL_VIEWPORT, oldVp);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_renderWidth, m_renderHeight);
    glClearColor(0.08F, 0.09F, 0.11F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, oldFb);
    glViewport(oldVp[0], oldVp[1], oldVp[2], oldVp[3]);

    ImGui::Image((ImTextureID)(intptr_t)m_colorTexture, canvasSize, ImVec2(0, 1), ImVec2(1, 0));

    const bool hovered = ImGui::IsItemHovered();
    const bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    const bool dragging = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
    const bool released = ImGui::IsMouseReleased(ImGuiMouseButton_Left);

    const ImVec2 mouse = ImGui::GetIO().MousePos;
    Engine::Core::Vec2 mouseWorld = screenToWorld(mouse.x - canvasPos.x, mouse.y - canvasPos.y, canvasSize.x, canvasSize.y);

    if (hovered && ImGui::GetIO().MouseWheel != 0.0F)
    {
        m_viewState->zoom = std::max(0.05F, m_viewState->zoom + ImGui::GetIO().MouseWheel * 0.1F);
    }

    if (hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
    {
        const ImVec2 d = ImGui::GetIO().MouseDelta;
        m_viewState->cameraX -= d.x / m_viewState->zoom;
        m_viewState->cameraY += d.y / m_viewState->zoom;
    }

    auto routes = m_basics->routeAllEdges();
    Render::Scene::SceneBuilder sceneBuilder;
    Render::Scene::RenderScene scene = sceneBuilder.build(m_basics->model(), routes);
    Render::Renderers::CanvasRenderer renderer;
    auto frame = renderer.buildFrame(scene);

    ImDrawList* dl = ImGui::GetWindowDrawList();

    if (m_basics->gridSettings().enabled)
    {
        const float s = m_basics->gridSettings().cellSize;
        for (float x = -4096.0F; x <= 4096.0F; x += s)
        {
            const glm::vec4 a = m_camera.viewProjection() * glm::vec4(x, -4096.0F, 0.0F, 1.0F);
            const glm::vec4 b = m_camera.viewProjection() * glm::vec4(x, 4096.0F, 0.0F, 1.0F);
            ImVec2 p0(canvasPos.x + (a.x / a.w + 1.0F) * 0.5F * canvasSize.x, canvasPos.y + (1.0F - (a.y / a.w + 1.0F) * 0.5F) * canvasSize.y);
            ImVec2 p1(canvasPos.x + (b.x / b.w + 1.0F) * 0.5F * canvasSize.x, canvasPos.y + (1.0F - (b.y / b.w + 1.0F) * 0.5F) * canvasSize.y);
            dl->AddLine(p0, p1, IM_COL32(40, 44, 48, 180), 1.0F);
        }
    }

    Render::Theme::NodeVisualRegistry visuals;

    for (Engine::Core::CanvasNodeId nodeId : m_basics->nodeIds())
    {
        const Engine::Core::Node* node = m_basics->model().getNode(nodeId);
        if (node == nullptr) continue;
        const auto& v = visuals.getVisual(mapType(*node));

        const glm::vec4 a = m_camera.viewProjection() * glm::vec4(node->position.x, node->position.y, 0.0F, 1.0F);
        const glm::vec4 b = m_camera.viewProjection() * glm::vec4(node->position.x + node->size.x, node->position.y + node->size.y, 0.0F, 1.0F);
        ImVec2 p0(canvasPos.x + (a.x / a.w + 1.0F) * 0.5F * canvasSize.x, canvasPos.y + (1.0F - (a.y / a.w + 1.0F) * 0.5F) * canvasSize.y);
        ImVec2 p1(canvasPos.x + (b.x / b.w + 1.0F) * 0.5F * canvasSize.x, canvasPos.y + (1.0F - (b.y / b.w + 1.0F) * 0.5F) * canvasSize.y);
        ImVec2 mn(std::min(p0.x, p1.x), std::min(p0.y, p1.y));
        ImVec2 mx(std::max(p0.x, p1.x), std::max(p0.y, p1.y));

        dl->AddRectFilled(mn, mx, color(v.bodyColor), v.cornerRadius);
        dl->AddRectFilled(mn, ImVec2(mx.x, mn.y + v.titleBarHeight), color(v.titleBarColor), v.cornerRadius);
        dl->AddRect(mn, mx, color(v.borderColor), v.cornerRadius, 0, v.borderThickness);
        if (nodeId == m_dragNode) dl->AddRect(mn, mx, color(v.selectionColor), v.cornerRadius, 0, 2.0F);

        const std::string label = v.icon.empty() ? v.title : (v.icon + std::string(" ") + v.title);
        dl->AddText(ImVec2(mn.x + 6.0F, mn.y + 4.0F), color(v.titleTextColor), label.c_str());

        if (clicked)
        {
            if (mouseWorld.x >= node->position.x && mouseWorld.x <= node->position.x + node->size.x &&
                mouseWorld.y >= node->position.y && mouseWorld.y <= node->position.y + node->size.y)
            {
                m_dragNode = nodeId;
                m_dragOffset = {mouseWorld.x - node->position.x, mouseWorld.y - node->position.y};
            }
        }

        for (std::size_t i = 0; i < node->connectors.size(); ++i)
        {
            const Engine::Core::Vec2 cw = connectorWorld(*node, i);
            const glm::vec4 cp = m_camera.viewProjection() * glm::vec4(cw.x, cw.y, 0.0F, 1.0F);
            ImVec2 sp(canvasPos.x + (cp.x / cp.w + 1.0F) * 0.5F * canvasSize.x,
                      canvasPos.y + (1.0F - (cp.y / cp.w + 1.0F) * 0.5F) * canvasSize.y);
            const ImU32 cc = (i % 2U == 0U) ? color(v.connectorInputColor) : color(v.connectorOutputColor);
            dl->AddCircleFilled(sp, 4.0F, cc);

            if (clicked)
            {
                const float dx = mouse.x - sp.x;
                const float dy = mouse.y - sp.y;
                if (dx * dx + dy * dy < 64.0F)
                {
                    const Engine::Core::CanvasConnectorId picked = node->connectors[i];
                    if (!m_pendingConnector.isValid())
                    {
                        m_pendingConnector = picked;
                    }
                    else if (m_pendingConnector != picked)
                    {
                        (void)m_basics->connect(m_pendingConnector, picked);
                        m_pendingConnector = {};
                    }
                }
            }
        }
    }

    for (const auto& edge : frame.edges)
    {
        for (std::size_t i = 0; i + 1 < edge.points.size(); ++i)
        {
            const glm::vec4 p0 = m_camera.viewProjection() * glm::vec4(edge.points[i].x, edge.points[i].y, 0.0F, 1.0F);
            const glm::vec4 p1 = m_camera.viewProjection() * glm::vec4(edge.points[i + 1].x, edge.points[i + 1].y, 0.0F, 1.0F);
            ImVec2 a(canvasPos.x + (p0.x / p0.w + 1.0F) * 0.5F * canvasSize.x, canvasPos.y + (1.0F - (p0.y / p0.w + 1.0F) * 0.5F) * canvasSize.y);
            ImVec2 b(canvasPos.x + (p1.x / p1.w + 1.0F) * 0.5F * canvasSize.x, canvasPos.y + (1.0F - (p1.y / p1.w + 1.0F) * 0.5F) * canvasSize.y);
            dl->AddLine(a, b, IM_COL32(220, 220, 235, 210), 2.0F);
        }
    }

    if (dragging && m_dragNode.isValid())
    {
        if (Engine::Core::Node* node = m_basics->model().getNode(m_dragNode); node != nullptr)
        {
            Engine::Core::Vec2 p{mouseWorld.x - m_dragOffset.x, mouseWorld.y - m_dragOffset.y};
            if (m_basics->gridSettings().snapEnabled)
            {
                const float s = m_basics->gridSettings().cellSize;
                p.x = std::round(p.x / s) * s;
                p.y = std::round(p.y / s) * s;
            }
            node->setPosition(p);
        }
    }
    if (released)
    {
        m_dragNode = {};
    }

    if (hovered && ImGui::IsKeyPressed(ImGuiKey_Delete) && m_dragNode.isValid())
    {
        m_basics->deleteNode(m_dragNode);
        m_dragNode = {};
    }

    ImGui::End();
}
} // namespace ScopeCanvas::Studio

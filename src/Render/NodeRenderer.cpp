#include "NodeRenderer.h"

#include <dlfcn.h>
#include <iostream>

namespace Render {

namespace {
constexpr float kBaseR = 0.18F;
constexpr float kBaseG = 0.23F;
constexpr float kBaseB = 0.30F;
constexpr float kSelectedR = 0.96F;
constexpr float kSelectedG = 0.64F;
constexpr float kSelectedB = 0.18F;
constexpr unsigned int kGlTriangles = 0x0004U;

using GlBeginFn = void (*)(unsigned int);
using GlEndFn = void (*)();
using GlColor3fFn = void (*)(float, float, float);
using GlVertex2fFn = void (*)(float, float);
using GlXGetCurrentContextFn = void* (*)();

class OpenGLDispatch final {
public:
    OpenGLDispatch() {
        m_glLib = dlopen("libGL.so.1", RTLD_LAZY);
        if (m_glLib == nullptr) {
            return;
        }

        m_glBegin = reinterpret_cast<GlBeginFn>(dlsym(m_glLib, "glBegin"));
        m_glEnd = reinterpret_cast<GlEndFn>(dlsym(m_glLib, "glEnd"));
        m_glColor3f = reinterpret_cast<GlColor3fFn>(dlsym(m_glLib, "glColor3f"));
        m_glVertex2f = reinterpret_cast<GlVertex2fFn>(dlsym(m_glLib, "glVertex2f"));
        m_glXGetCurrentContext = reinterpret_cast<GlXGetCurrentContextFn>(dlsym(m_glLib, "glXGetCurrentContext"));
    }

    ~OpenGLDispatch() {
        if (m_glLib != nullptr) {
            dlclose(m_glLib);
        }
    }

    [[nodiscard]] bool canDraw() const noexcept {
        if (m_glBegin == nullptr || m_glEnd == nullptr || m_glColor3f == nullptr || m_glVertex2f == nullptr || m_glXGetCurrentContext == nullptr) {
            return false;
        }

        return m_glXGetCurrentContext() != nullptr;
    }

    void submitTriangles(const std::vector<NodeVertex>& vertices) const {
        if (!canDraw()) {
            std::cout << "[Render] nodes OpenGL unavailable (no current context)\n";
            return;
        }

        m_glBegin(kGlTriangles);
        for (const NodeVertex& v : vertices) {
            m_glColor3f(v.r, v.g, v.b);
            m_glVertex2f(v.x, v.y);
        }
        m_glEnd();
    }

private:
    void* m_glLib {nullptr};
    GlBeginFn m_glBegin {nullptr};
    GlEndFn m_glEnd {nullptr};
    GlColor3fFn m_glColor3f {nullptr};
    GlVertex2fFn m_glVertex2f {nullptr};
    GlXGetCurrentContextFn m_glXGetCurrentContext {nullptr};
};
}

void NodeRenderer::draw(const Engine::DiagramModel& model, const View::Camera2D& camera) {
    rebuildVertexBuffer(model, camera);

    static OpenGLDispatch gl;
    gl.submitTriangles(m_vbo);

    std::cout << "[Render] nodes count=" << model.nodes().size() << " batched_vertices=" << m_vbo.size() << '\n';
}

void NodeRenderer::rebuildVertexBuffer(const Engine::DiagramModel& model, const View::Camera2D& camera) {
    m_vbo.clear();
    m_vbo.reserve(model.nodes().size() * 6U);

    for (const Engine::Node& node : model.nodes()) {
        const Engine::Vec2 topLeft = camera.worldToView(node.position);
        const float w = node.size.x * camera.zoom();
        const float h = node.size.y * camera.zoom();

        const float r = node.selected ? kSelectedR : kBaseR;
        const float g = node.selected ? kSelectedG : kBaseG;
        const float b = node.selected ? kSelectedB : kBaseB;

        const NodeVertex v0 {topLeft.x, topLeft.y, r, g, b};
        const NodeVertex v1 {topLeft.x + w, topLeft.y, r, g, b};
        const NodeVertex v2 {topLeft.x + w, topLeft.y + h, r, g, b};
        const NodeVertex v3 {topLeft.x, topLeft.y + h, r, g, b};

        m_vbo.insert(m_vbo.end(), {v0, v1, v2, v0, v2, v3});
    }
}

} // namespace Render

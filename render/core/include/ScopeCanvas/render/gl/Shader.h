#pragma once

#include <glad/glad.h>

namespace ScopeCanvas::Render::GL {
class Shader {

  private:
    GLuint m_programId;

  public:
    Shader() : m_programId(0) {}
    ~Shader() {
        destroy();
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept : m_programId(other.m_programId) {
        other.m_programId = 0;
    }

    Shader& operator=(Shader&& other) noexcept;

    bool load(const char* vertexSrc, const char* fragmentSrc);

    void use() const {
        glUseProgram(m_programId);
    }

    GLuint id() const {
        return m_programId;
    }

  private:
    bool compileShader(GLuint shaderId, const char* source) const;

    void destroy() {
        if (m_programId != 0) {
            glDeleteProgram(m_programId);
            m_programId = 0;
        }
    }
};

extern const char GridVertex[];
extern const char GridFragment[];
extern const char EdgeVertex[];
extern const char EdgeFragment[];
extern const char SelectionRectVertex[];
extern const char SelectionRectFragment[];
} // namespace ScopeCanvas::Render::GL

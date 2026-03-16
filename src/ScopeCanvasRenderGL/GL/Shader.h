#pragma once

#include <glad/glad.h>

namespace ScopeCanvas::Render::GL
{
class Shader
{
public:
    Shader();
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    bool load(const char* vertexSrc, const char* fragmentSrc);
    void use() const;

    GLuint id() const;

private:
    bool compileShader(GLuint shaderId, const char* source) const;
    void destroy();

    GLuint m_programId;
};

extern const char GridVertex[];
extern const char GridFragment[];
extern const char EdgeVertex[];
extern const char EdgeFragment[];
} // namespace ScopeCanvas::Render::GL

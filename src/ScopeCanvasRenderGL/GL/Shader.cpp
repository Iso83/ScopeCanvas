#include "GL/Shader.h"

namespace ScopeCanvas::RenderGL::GL
{
bool Shader::compile(const std::string& vertexSource, const std::string& fragmentSource)
{
    m_compiled = !vertexSource.empty() && !fragmentSource.empty();
    return m_compiled;
}

bool Shader::isCompiled() const
{
    return m_compiled;
}
} // namespace ScopeCanvas::RenderGL::GL

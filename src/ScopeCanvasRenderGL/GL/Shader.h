#pragma once

#include <string>

namespace ScopeCanvas::RenderGL::GL
{
class Shader
{
public:
    bool compile(const std::string& vertexSource, const std::string& fragmentSource);
    [[nodiscard]] bool isCompiled() const;

private:
    bool m_compiled{false};
};
} // namespace ScopeCanvas::RenderGL::GL

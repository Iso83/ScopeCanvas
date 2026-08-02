#pragma once

namespace ScopeCanvas::Engine::Render::GL {
#if defined(__EMSCRIPTEN__)
inline constexpr const char* ShaderVersionPrefix =
    "#version 300 es\nprecision mediump float;\nprecision mediump int;\n";
#else
inline constexpr const char* ShaderVersionPrefix = "#version 330 core\n";
#endif
} // namespace ScopeCanvas::Engine::Render::GL

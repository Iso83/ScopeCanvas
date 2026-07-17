#include <ScopeCanvas/render/gl/Shader.h>
#include <ScopeCanvas/render/gl/ShaderSource.h>
#include <iostream>
#include <string>

namespace ScopeCanvas::Render::GL {
Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        destroy();
        m_programId = other.m_programId;
        other.m_programId = 0;
    }
    return *this;
}

bool Shader::load(const char* vertexSrc, const char* fragmentSrc) {
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    if (!compileShader(vertexShader, vertexSrc) || !compileShader(fragmentShader, fragmentSrc)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    destroy();
    m_programId = glCreateProgram();
    glAttachShader(m_programId, vertexShader);
    glAttachShader(m_programId, fragmentShader);
    glLinkProgram(m_programId);

    GLint success = 0;
    glGetProgramiv(m_programId, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512]{};
        glGetProgramInfoLog(m_programId, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader program link failed: " << infoLog << '\n';
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        destroy();
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

bool Shader::compileShader(GLuint shaderId, const char* source) const {
    glShaderSource(shaderId, 1, &source, nullptr);
    glCompileShader(shaderId);

    GLint success = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512]{};
        glGetShaderInfoLog(shaderId, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader compile failed: " << infoLog << '\n';
        return false;
    }

    return true;
}


namespace {
const std::string& gridVertexStorage() {
    static const std::string source = std::string(ShaderVersionPrefix) + R"(layout(location = 0) in vec2 aWorldPos;

uniform mat4 uViewProjection;

void main()
{
    gl_Position = uViewProjection * vec4(aWorldPos, 0.0, 1.0);
}
)";
    return source;
}
const std::string& gridFragmentStorage() {
    static const std::string source = std::string(ShaderVersionPrefix) + R"(out vec4 FragColor;

uniform vec3 uColor;

void main()
{
    FragColor = vec4(uColor, 1.0);
}
)";
    return source;
}
const std::string& edgeVertexStorage() {
    static const std::string source = std::string(ShaderVersionPrefix) + R"(layout(location = 0) in vec2 aPosition;

uniform mat4 uViewProjection;

void main() {
    gl_Position = uViewProjection * vec4(aPosition, 0.0, 1.0);
}
)";
    return source;
}
const std::string& edgeFragmentStorage() {
    static const std::string source = std::string(ShaderVersionPrefix) + R"(out vec4 FragColor;

uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}
)";
    return source;
}
const std::string& selectionRectVertexStorage() {
    static const std::string source = std::string(ShaderVersionPrefix) + R"(layout (location = 0) in vec2 aLocalPos;

uniform mat4 uViewProjection;
uniform vec2 uRectPosition;
uniform vec2 uRectSize;

void main() {
    vec2 worldPos = uRectPosition + (aLocalPos * uRectSize);
    gl_Position = uViewProjection * vec4(worldPos, 0.0, 1.0);
}
)";
    return source;
}
const std::string& selectionRectFragmentStorage() {
    static const std::string source = std::string(ShaderVersionPrefix) + R"(out vec4 FragColor;

uniform vec4 uColor;

void main() {
    FragColor = uColor;
}
)";
    return source;
}
} // namespace

const char* gridVertexSource() { return gridVertexStorage().c_str(); }
const char* gridFragmentSource() { return gridFragmentStorage().c_str(); }
const char* edgeVertexSource() { return edgeVertexStorage().c_str(); }
const char* edgeFragmentSource() { return edgeFragmentStorage().c_str(); }
const char* selectionRectVertexSource() { return selectionRectVertexStorage().c_str(); }
const char* selectionRectFragmentSource() { return selectionRectFragmentStorage().c_str(); }
} // namespace ScopeCanvas::Render::GL

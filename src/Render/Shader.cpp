#include "Render/Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

namespace {
std::string loadFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "Failed to open shader file: " << path << '\n';
        return {};
    }
    std::cout << "Loading shader: " << path << std::endl;
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}
}

Shader::Shader() : m_programId(0) {}

Shader::~Shader() {
    destroy();
}

Shader::Shader(Shader&& other) noexcept : m_programId(other.m_programId) {
    other.m_programId = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        destroy();
        m_programId = other.m_programId;
        other.m_programId = 0;
    }
    return *this;
}

bool Shader::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    const std::string vertexSource = loadFile(vertexPath);
    const std::string fragmentSource = loadFile(fragmentPath);
    if (vertexSource.empty() || fragmentSource.empty()) {
        std::cout << "Loading vertex shader: " << vertexPath << std::endl;
        std::cout << "Loading fragment shader: " << fragmentPath << std::endl;

        return false;
    }

    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    if (!compileShader(vertexShader, vertexSource) || !compileShader(fragmentShader, fragmentSource)) {
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

void Shader::use() const {
    glUseProgram(m_programId);
}

bool Shader::compileShader(GLuint shaderId, const std::string& source) const {
    const char* src = source.c_str();
    glShaderSource(shaderId, 1, &src, nullptr);
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

void Shader::destroy() {
    if (m_programId != 0) {
        glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

#pragma once

#include <glad/glad.h>

#include <string>

class Shader {
public:
    Shader();
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;

    GLuint id() const { return m_programId; }

private:
    bool compileShader(GLuint shaderId, const std::string& source) const;
    void destroy();

    GLuint m_programId;
};

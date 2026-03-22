#include "Render/Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

Shader::Shader() : m_programId(0) {}

Shader::~Shader() {
	destroy();
}

Shader::Shader(Shader &&other) noexcept : m_programId(other.m_programId) {
	other.m_programId = 0;
}

Shader &Shader::operator=(Shader &&other) noexcept {
	if (this != &other) {
		destroy();
		m_programId = other.m_programId;
		other.m_programId = 0;
	}
	return *this;
}

bool Shader::load(const char *vertexSrc, const char *fragmentSrc) {
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

void Shader::use() const {
	glUseProgram(m_programId);
}

bool Shader::compileShader(GLuint shaderId, const char *src) const {
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

const char GridVertex[] = R"(
	#version 330 core
	layout(location = 0) in vec2 aWorldPos;

	uniform mat4 uViewProjection;

	void main()
	{
		gl_Position = uViewProjection * vec4(aWorldPos, 0.0, 1.0);
	}
)";
const char GridFragment[] = R"(
	#version 330 core
	out vec4 FragColor;

	uniform vec3 uColor;

	void main()
	{
		FragColor = vec4(uColor, 1.0);
	}
)";


const char NodeVertex[] = R"(
	#version 330 core
	layout (location = 0) in vec2 aLocalPos;

	uniform mat4 uViewProjection;
	uniform vec2 uNodePosition;
	uniform vec2 uNodeSize;

	void main() {
		vec2 worldPos = uNodePosition + (aLocalPos * uNodeSize);
		gl_Position = uViewProjection * vec4(worldPos, 0.0, 1.0);
	}
)";
const char NodeFragment[] = R"(
	#version 330 core
	out vec4 FragColor;

	uniform vec3 uNodeColor;

	void main() {
		FragColor = vec4(uNodeColor, 1.0);
	}
)";

const char EdgeVertex[] = R"(
	#version 330 core
	layout(location = 0) in vec2 aPosition;

	uniform mat4 uViewProjection;

	void main() {
		gl_Position = uViewProjection * vec4(aPosition, 0.0, 1.0);
	}
)";
const char EdgeFragment[] = R"(
	#version 330 core
	out vec4 FragColor;

	uniform vec3 uColor;

	void main() {
		FragColor = vec4(uColor, 1.0);
	}
)";

const char SelectionRectVertex[] = R"(
	#version 330 core
	layout (location = 0) in vec2 aLocalPos;

	uniform mat4 uViewProjection;
	uniform vec2 uRectPosition;
	uniform vec2 uRectSize;

	void main() {
		vec2 worldPos = uRectPosition + (aLocalPos * uRectSize);
		gl_Position = uViewProjection * vec4(worldPos, 0.0, 1.0);
	}
)";
const char SelectionRectFragment[] = R"(
	#version 330 core
	out vec4 FragColor;

	uniform vec4 uColor;

	void main() {
		FragColor = uColor;
	}
)";
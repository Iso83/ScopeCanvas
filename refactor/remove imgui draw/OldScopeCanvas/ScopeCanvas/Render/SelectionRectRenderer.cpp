#include "Render/SelectionRectRenderer.h"

#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

SelectionRectRenderer::SelectionRectRenderer() : m_vao(0), m_vbo(0), m_fillEbo(0), m_outlineEbo(0) {}

SelectionRectRenderer::~SelectionRectRenderer() {
	destroy();
}

bool SelectionRectRenderer::init() {
	if (!m_shader.load(SelectionRectVertex, SelectionRectFragment))
		return false;

	constexpr float vertices[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};

	constexpr GLuint fillIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	constexpr GLuint outlineIndices[] = {
		0, 1, 2, 3,
	};

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_fillEbo);
	glGenBuffers(1, &m_outlineEbo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fillEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fillIndices), fillIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_outlineEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(outlineIndices), outlineIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

	glBindVertexArray(0);
	return true;
}

void SelectionRectRenderer::render(const glm::mat4 &viewProjection,
	const glm::vec2 &start,
	const glm::vec2 &end) const {
	const glm::vec2 rectMin(glm::min(start.x, end.x), glm::min(start.y, end.y));
	const glm::vec2 rectSize(glm::max(start.x, end.x) - rectMin.x, glm::max(start.y, end.y) - rectMin.y);

	m_shader.use();

	const GLuint programId = m_shader.id();
	const GLint vpLoc = glGetUniformLocation(programId, "uViewProjection");
	const GLint posLoc = glGetUniformLocation(programId, "uRectPosition");
	const GLint sizeLoc = glGetUniformLocation(programId, "uRectSize");
	const GLint colorLoc = glGetUniformLocation(programId, "uColor");

	glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(viewProjection));
	glUniform2f(posLoc, rectMin.x, rectMin.y);
	glUniform2f(sizeLoc, rectSize.x, rectSize.y);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fillEbo);
	glUniform4f(colorLoc, 0.7f, 0.8f, 1.0f, 0.1f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_outlineEbo);
	glUniform4f(colorLoc, 0.7f, 0.8f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

void SelectionRectRenderer::destroy() {
	if (m_outlineEbo != 0) {
		glDeleteBuffers(1, &m_outlineEbo);
		m_outlineEbo = 0;
	}

	if (m_fillEbo != 0) {
		glDeleteBuffers(1, &m_fillEbo);
		m_fillEbo = 0;
	}

	if (m_vbo != 0) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	if (m_vao != 0) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
}

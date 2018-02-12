#include "Debug.h"

#include "ShaderManager.h"

#include "GLW/ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <iomanip>
#include <iostream>


//=================================================================================
bool _glErrorCheck(const std::string file, const int line) {

 GLenum status;
 while ((status = glGetError()) != 0x0000) {
	 std::cout << "[" << file.substr(file.rfind("\\") + 1) << ":" << line << "] Error (0x"
		 << std::hex << std::setfill('0') << std::setw(4)
		 << status << "): " << glewGetErrorString(status)
		 << std::dec
		 << std::endl;
#if _DEBUG && 0
	 __debugbreak();
#endif
	 return true;
 }
 return false;
}

//=================================================================================
C_DebugDraw & C_DebugDraw::Instance()
{
	static C_DebugDraw    instance; // Guaranteed to be destroyed.
									// Instantiated on first use.
	return instance;
}
#if _DEBUG
//=================================================================================
void C_DebugDraw::SetupAABB()
{
	m_program = C_ShaderManager::Instance().GetProgram("basic-wireframe");

	glGenVertexArrays(1, &m_VAOaabb);
	glBindVertexArray(m_VAOaabb);
	// Cube 1x1x1, centered on origin
	GLfloat vertices[] = {
		-0.5, -0.5, -0.5, 1.0,
		0.5, -0.5, -0.5, 1.0,
		0.5,  0.5, -0.5, 1.0,
		-0.5,  0.5, -0.5, 1.0,
		-0.5, -0.5,  0.5, 1.0,
		0.5, -0.5,  0.5, 1.0,
		0.5,  0.5,  0.5, 1.0,
		-0.5,  0.5,  0.5, 1.0,
	};
	glGenBuffers(1, &m_VBOaabb);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOaabb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	ErrorCheck();
	GLushort elements[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5, 2, 6, 3, 7
	};
	glGenBuffers(1, &m_IBOaabb);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBOaabb);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//=================================================================================
C_DebugDraw::C_DebugDraw()
{
	SetupAABB();
	glGenVertexArrays(1, &m_VAOline);
	glBindVertexArray(m_VAOline);
	glGenBuffers(1, &m_VBOline);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOline);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//=================================================================================
C_DebugDraw::~C_DebugDraw()
{
	//we cannot do this because OpenGL contex is already death now
	//ErrorCheck();
	//glDeleteBuffers(1, &m_VBO);
	//ErrorCheck();
	//glDeleteBuffers(1, &m_IBO);
	//ErrorCheck();
	//glDeleteVertexArrays(1, &m_VAO);
	//ErrorCheck();
}

//=================================================================================
void C_DebugDraw::DrawPoint(const glm::vec4 & point, const glm::mat4 & projectionMatrix, const glm::vec3 & color, const glm::mat4 & modelMatrix)
{
	m_program->useProgram();

	glBindVertexArray(m_VAOline);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOline);

	glPointSize(5.0f);
	ErrorCheck();

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), glm::value_ptr(point), GL_DYNAMIC_DRAW);
	ErrorCheck();

	glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniform3fv(glGetUniformLocation(m_program->GetProgram(), "colorIN"), 1, glm::value_ptr(color));
	ErrorCheck();

	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, 1);
	ErrorCheck();

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_program->disableProgram();
	ErrorCheck();
}

//=================================================================================
void C_DebugDraw::DrawPoint(const glm::vec3 & point, const glm::mat4 & projectionMatrix, const glm::vec3 & color, const glm::mat4 & modelMatrix)
{
	DrawPoint(toVec4(point), projectionMatrix, color, modelMatrix);
}

//=================================================================================
void C_DebugDraw::DrawAABB(const AABB& bbox, const glm::mat4& projectionMatrix, const glm::vec3& color /*= glm::vec3(0.0f, 0.0f, 0.0f)*/, const glm::mat4& modelMatrix /*= glm::mat4(1.0f)*/)
{
	m_program->useProgram();
	glm::vec3 size = bbox.maxPoint - bbox.minPoint;
	glm::vec3 center = (bbox.maxPoint + bbox.minPoint) / 2.0f;// glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

	/* Apply object's transformation matrix */
	glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix*transform));
	glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniform3fv(glGetUniformLocation(m_program->GetProgram(), "colorIN"), 1, glm::value_ptr(color));

	glBindVertexArray(m_VAOaabb);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOaabb);
	ErrorCheck();
	glEnableVertexAttribArray(0);
	ErrorCheck();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBOaabb);
	ErrorCheck();
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	ErrorCheck();
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
	ErrorCheck();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	ErrorCheck();

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_program->disableProgram();
	glBindVertexArray(0);
	ErrorCheck();
}

//=================================================================================
void C_DebugDraw::DrawLine(const glm::vec4& pointA, const glm::vec4& pointB, const glm::mat4& projectionMatrix, const glm::vec3& color /*= glm::vec3(0.0f, 0.0f, 0.0f)*/)
{
	m_program->useProgram();

	glBindVertexArray(m_VAOline);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOline);

	std::vector<glm::vec4> vertices;
	vertices.push_back(pointA);
	vertices.push_back(pointB);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), vertices.data(), GL_DYNAMIC_DRAW);

	glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniform3fv(glGetUniformLocation(m_program->GetProgram(), "colorIN"), 1, glm::value_ptr(color));

	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_program->disableProgram();
	ErrorCheck();
}

//=================================================================================
void C_DebugDraw::DrawLines(const std::vector<glm::vec4>& pairs, const glm::mat4 & projectionMatrix, const glm::vec3 & color)
{
	m_program->useProgram();

	glBindVertexArray(m_VAOline);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOline);

	std::vector<glm::vec4> vertices;

	glBufferData(GL_ARRAY_BUFFER, pairs.size() * sizeof(glm::vec4), pairs.data(), GL_DYNAMIC_DRAW);

	glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniform3fv(glGetUniformLocation(m_program->GetProgram(), "colorIN"), 1, glm::value_ptr(color));

	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, pairs.size());

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_program->disableProgram();
	ErrorCheck();
}

void C_DebugDraw::DrawAxis(const glm::vec4 & origin, const glm::vec4 & up, const glm::vec4 & foreward, const glm::mat4 & projectionMatrix, glm::mat4 & modelMatrix)
{
	glm::vec4 forewardVec = glm::normalize(foreward - origin);
	glm::vec4 upVec = glm::normalize(up - origin);
	glm::vec4 rightVec = toVec4(glm::normalize(glm::cross(glm::vec3(upVec), glm::vec3(forewardVec))));
	DrawLine(origin, origin + forewardVec,	projectionMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
	DrawLine(origin, origin + upVec,		projectionMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
	DrawLine(origin, origin + rightVec,		projectionMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
}

#endif
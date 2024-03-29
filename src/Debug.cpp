#include "Debug.h"

#include "ShaderManager.h"

#include "GLW/ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <iomanip>
#include <iostream>


//=================================================================================
const char* glErrorCodeToString(unsigned int code) {
#define codeToStr(c) case c: return #c; break
	switch (code)
	{
		codeToStr(GL_INVALID_ENUM);
		codeToStr(GL_INVALID_VALUE);
		codeToStr(GL_INVALID_OPERATION);
		codeToStr(GL_STACK_OVERFLOW);
		codeToStr(GL_STACK_UNDERFLOW);
		codeToStr(GL_OUT_OF_MEMORY);
	default:
		return "bad value";
		break;
	}

}

//=================================================================================
bool _glErrorCheck(const char* file, const int line) 
{
 GLenum status;
 while ((status = glGetError()) != GL_NO_ERROR) {
	 std::cout << "[" << file << ":" << line << "] Error (0x"
		 << std::hex << std::setfill('0') << std::setw(4)
		 << status << ":"<< glErrorCodeToString(status) << "): "
		 << glewGetErrorString(status)
		 << std::dec
		 << std::endl;
#if _DEBUG
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

	glEnableVertexAttribArray(0);
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

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//=================================================================================
C_DebugDraw::~C_DebugDraw()
{
}

//=================================================================================
void C_DebugDraw::Clear()
{
	glDeleteBuffers(1, &m_VBOline);
	glDeleteBuffers(1, &m_IBOaabb);
	glDeleteVertexArrays(1, &m_VAOline);
	glDeleteBuffers(1, &m_VBOaabb);
	glDeleteVertexArrays(1, &m_VAOaabb);
	ErrorCheck();
}

//=================================================================================
void C_DebugDraw::DrawPoint(const glm::vec4 & point, const glm::mat4 & projectionMatrix, const glm::vec3 & color, const glm::mat4 & modelMatrix)
{
	auto& shdManager = C_ShaderManager::Instance();
	auto program = shdManager.GetProgram("basic-wireframe");
	shdManager.ActivateShader(program);

	glBindVertexArray(m_VAOline);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOline);

	glPointSize(5.0f);
	ErrorCheck();

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), glm::value_ptr(point), GL_DYNAMIC_DRAW);
	ErrorCheck();

	program->SetUniform("modelMatrix", modelMatrix);
	program->SetUniform("projectionMatrix", projectionMatrix);
	program->SetUniform("colorIN", color);
	ErrorCheck();

	glDrawArrays(GL_POINTS, 0, 1);
	ErrorCheck();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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
	auto& shdManager = C_ShaderManager::Instance();
	auto program = C_ShaderManager::Instance().GetProgram("basic-wireframe");
	shdManager.ActivateShader(program);

	glm::vec3 size = bbox.maxPoint - bbox.minPoint;
	glm::vec3 center = (bbox.maxPoint + bbox.minPoint) / 2.0f;// glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

	/* Apply object's transformation matrix */
	program->SetUniform("modelMatrix", modelMatrix*transform);
	program->SetUniform("projectionMatrix", projectionMatrix);
	program->SetUniform("colorIN", color);

	glBindVertexArray(m_VAOaabb);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOaabb);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBOaabb);
	ErrorCheck();
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	ErrorCheck();
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
	ErrorCheck();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	ErrorCheck();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	ErrorCheck();
}

//=================================================================================
void C_DebugDraw::DrawLine(const glm::vec4& pointA, const glm::vec4& pointB, const glm::mat4& projectionMatrix, const glm::vec3& color /*= glm::vec3(0.0f, 0.0f, 0.0f)*/)
{
	auto& shdManager = C_ShaderManager::Instance();
	auto program = C_ShaderManager::Instance().GetProgram("basic-wireframe");
	shdManager.ActivateShader(program);

	glBindVertexArray(m_VAOline);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOline);

	std::vector<glm::vec4> vertices;
	vertices.push_back(pointA);
	vertices.push_back(pointB);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), vertices.data(), GL_DYNAMIC_DRAW);

	program->SetUniform("modelMatrix", glm::mat4(1.0f));
	program->SetUniform("projectionMatrix", projectionMatrix);
	program->SetUniform("colorIN", color);

	glDrawArrays(GL_LINES, 0, 2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	ErrorCheck();
}

//=================================================================================
void C_DebugDraw::DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const glm::mat4& projectionMatrix, const glm::vec3& color /*= glm::vec3(0.0f, 0.0f, 0.0f)*/)
{
	DrawLine(toVec4(pointA), toVec4(pointB), projectionMatrix, color);
}

//=================================================================================
void C_DebugDraw::DrawLines(const std::vector<glm::vec4>& pairs, const glm::mat4 & projectionMatrix, const glm::vec3 & color)
{
	auto& shdManager = C_ShaderManager::Instance();
	auto program = C_ShaderManager::Instance().GetProgram("basic-wireframe");
	shdManager.ActivateShader(program);

	glBindVertexArray(m_VAOline);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOline);

	std::vector<glm::vec4> vertices;
	ErrorCheck();

	glBufferData(GL_ARRAY_BUFFER, pairs.size() * sizeof(glm::vec4), pairs.data(), GL_DYNAMIC_DRAW);

	ErrorCheck();
	program->SetUniform("modelMatrix", glm::mat4(1.0f));
	program->SetUniform("projectionMatrix", projectionMatrix);
	program->SetUniform("colorIN", color);

	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(pairs.size()));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	ErrorCheck();
}

//=================================================================================
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
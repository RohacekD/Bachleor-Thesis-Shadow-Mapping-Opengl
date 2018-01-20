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

 GLenum status = glGetError();
 if (status != 0x0000) {
	 std::cout << "[" << file.substr(file.rfind("\\") + 1) << ":" << line << "] Error (0x"
		 << std::hex << std::setfill('0') << std::setw(4)
		 << status << "): " << glewGetErrorString(status)
		 << std::dec
		 << std::endl;
	 __debugbreak();
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

//=================================================================================
C_DebugDraw::C_DebugDraw()
{
	m_program = C_ShaderManager::Instance().GetProgram("basic-wireframe");

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
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
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	ErrorCheck();
	GLushort elements[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5, 2, 6, 3, 7
	};
	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
void C_DebugDraw::DrawAABB(const AABB& bbox, const glm::mat4& modelMatrix, const glm::mat4& projectionMatrix, const glm::vec3& color)
{
	m_program->useProgram();
	glm::vec3 size = bbox.maxPoint - bbox.minPoint;
	glm::vec3 center = (bbox.maxPoint + bbox.minPoint) / 2.0f;// glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

	/* Apply object's transformation matrix */
	glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix*transform));
	glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniform3fv(glGetUniformLocation(m_program->GetProgram(), "colorIN"), 1, glm::value_ptr(color));

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	ErrorCheck();
	glEnableVertexAttribArray(0);
	ErrorCheck();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
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
	ErrorCheck();
}

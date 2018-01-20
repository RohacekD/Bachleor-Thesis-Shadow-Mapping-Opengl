#pragma once
#include <string>
#include <memory>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "Scene.hpp"

//=================================================================================
//= returns true if error occurs
//=================================================================================
bool _glErrorCheck(const std::string file, const int line);

//=================================================================================
#define ErrorCheck() _glErrorCheck(__FILE__, __LINE__)

//=================================================================================

namespace GLW {
	class C_ShaderProgram;
}

//=================================================================================
class C_DebugDraw {
public:
	//Singleton stuff
	C_DebugDraw(C_DebugDraw const&) = delete;
	void operator=(C_DebugDraw const&) = delete;
	static C_DebugDraw& Instance();
	~C_DebugDraw();

	void DrawAABB(const AABB& bbox, const glm::mat4& modelMatrix, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f));

private:
	std::shared_ptr<GLW::C_ShaderProgram> m_program;
	C_DebugDraw();
	GLuint m_IBO;
	GLuint m_VBO;
	GLuint m_VAO;

};
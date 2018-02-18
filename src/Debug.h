#pragma once

#include "RenderDocTools.h"

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "Scene.hpp"

#include <string>
#include <memory>
#include <iostream>


//=================================================================================
//= returns true if error occurs
//=================================================================================
bool _glErrorCheck(const std::string file, const int line);

//=================================================================================
#define ErrorCheck() _glErrorCheck(__FILE__, __LINE__)

//=================================================================================
#if _DEBUG
#define DestructorFullCheck() _glErrorCheck(__FILE__, __LINE__); std::cout<<__FUNCTION__<<std::endl
#else
#define DestructorFullCheck()
#endif

//=================================================================================

namespace GLW {
	class C_ShaderProgram;
}

#if _DEBUG
//=================================================================================
class C_DebugDraw {
public:
	//Singleton stuff
	C_DebugDraw(C_DebugDraw const&) = delete;
	void operator=(C_DebugDraw const&) = delete;
	static C_DebugDraw& Instance();
	~C_DebugDraw();

	void Clear();


	void DrawPoint(const glm::vec3& point, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f));
	void DrawPoint(const glm::vec4& point, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f));
	void DrawAABB(const AABB& bbox, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f));
	void DrawLine(const glm::vec4& pointA, const glm::vec4& pointB, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f));
	void DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f));
	void DrawLines(const std::vector<glm::vec4>& pairs, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f));

	void DrawAxis(const glm::vec4& origin, const glm::vec4& up, const glm::vec4& foreward, const glm::mat4& projectionMatrix, glm::mat4& modelMatrix = glm::mat4(1.0f));
private:
	inline glm::vec4 toVec4(const glm::vec3& vec) const { return glm::vec4(vec, 1.0f); };
	void SetupAABB();

	std::shared_ptr<GLW::C_ShaderProgram> m_program;
	C_DebugDraw();
	GLuint m_IBOaabb;
	GLuint m_VBOaabb;
	GLuint m_VAOaabb;

	GLuint m_VAOline;
	GLuint m_VBOline;
};
#else
//=================================================================================
class C_DebugDraw {
public:
	//Singleton stuff
	C_DebugDraw(C_DebugDraw const&) = delete;
	void operator=(C_DebugDraw const&) = delete;
	static C_DebugDraw& Instance();
	~C_DebugDraw() {};

	void Clear() {};

	void DrawPoint(const glm::vec3& point, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f)) {};
	void DrawPoint(const glm::vec4& point, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f)) {};
	void DrawAABB(const AABB& bbox, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f)) {};
	void DrawLine(const glm::vec4& pointA, const glm::vec4& pointB, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f)) {};
	void DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f)) {};
	void DrawLines(const std::vector<glm::vec4>& pairs, const glm::mat4& projectionMatrix, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f)) {};

	void DrawAxis(const glm::vec4& origin, const glm::vec4& up, const glm::vec4& foreward, const glm::mat4& projectionMatrix, glm::mat4& modelMatrix = glm::mat4(1.0f)) {};
private:

	C_DebugDraw() {};
};
#endif
#include "MeshNode.h"
#include "Debug.h"
#include "Scene.hpp"
#include "ShaderCompiler.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace render {

	//=================================================================================
	C_MeshNode::C_MeshNode()
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_VBOs[0]);
		glGenBuffers(1, &m_VBOs[1]);
		glGenBuffers(1, &m_VBOs[2]);
		glBindVertexArray(0);
	}

	//=================================================================================
	C_MeshNode::~C_MeshNode()
	{
	}

	//=================================================================================
	void C_MeshNode::Render(const S_RenderParams& params)
{
		glUseProgram(m_program);
		glBindVertexArray(m_VAO);
		glEnable(GL_DEPTH_TEST);
		// glCullFace(GL_FRONT);
		// bind VBOs for vertex array and index array
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glActiveTexture(GL_TEXTURE0);
		m_texture->bind();
		glUniform1i(glGetUniformLocation(m_program, "tex"), 0);

		glm::vec4 light(0, 0, 15.f, 1.0f);

		glFrontFace(GL_CCW);
		glUniformMatrix4fv(glGetUniformLocation(m_program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(cameraViewProjectionMatrix));
		glUniform4fv(glGetUniformLocation(m_program, "lightPos"), 1, glm::value_ptr(light));

		glDrawArrays(GL_TRIANGLES, 0, m_triangles);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		T_Base::Render(params);
	}

	//=================================================================================
	void C_MeshNode::Clean()
	{
		T_Base::Clean();
		m_texture.reset();
		glDeleteBuffers(1, &m_VBOs[0]);
		glDeleteBuffers(1, &m_VBOs[1]);
		glDeleteBuffers(1, &m_VBOs[2]);
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteProgram(m_program);
	}

	//=================================================================================
	void C_MeshNode::LoadMesh(const Mesh & mesh)
	{
		m_triangles = static_cast<GLuint>(mesh.vertices.size());
		glBindVertexArray(m_VAO);

		// bind VBO in order to use
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);

		static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support this directly.");

		// upload data to VBO
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec4), mesh.vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		ErrorCheck();


		glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
		// upload data to VBO
		glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), mesh.normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[2]);
		// upload data to VBO
		glBufferData(GL_ARRAY_BUFFER, mesh.texcoords.size() * sizeof(glm::vec2), mesh.texcoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		ShaderCompiler compiler;
		GLuint vertexShader;
		std::string str;
		if (!compiler.compileShader(vertexShader, "shaders/basic/vertex.glsl", GL_VERTEX_SHADER, str)) {
			std::cerr << str << std::endl;
			return;// false;
		}

		GLuint fragmentShader;
		if (!compiler.compileShader(fragmentShader, "shaders/basic/fragment.glsl", GL_FRAGMENT_SHADER, str)) {
			std::cerr << str << std::endl;
			return;// false;
		}

		if (!compiler.linkProgram(m_program, str, 2, vertexShader, fragmentShader)) {
			std::cerr << str << std::endl;
			return;// false;
		}
	}
};
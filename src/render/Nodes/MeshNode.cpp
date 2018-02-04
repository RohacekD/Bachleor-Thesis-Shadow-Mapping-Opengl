#include "MeshNode.h"
#include "Debug.h"
#include "Scene.hpp"
#include "ShaderManager.h"
#include "GLW/ShaderProgram.h"

#include "CameraManager.h"
#include "Camera/ICamera.h"
#include "Application.hpp"

#include "render/RenderParams.h"

#include "animation/Animation.h"
#include "animation/TranslationAnimations.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>

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
		if (params.m_pass == render::S_RenderParams::E_PassType::E_P_RenderPass) {
			RenderPass(params);
		}
		else {
			ShadowPass(params);
		}

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
	}

	//=================================================================================
	void C_MeshNode::LoadMesh(const Mesh & mesh)
	{
		m_triangles = static_cast<GLuint>(mesh.vertices.size());
		m_bbox = mesh.bbox;

		// ----------- just for test -------------
		//if (m_triangles > 6) {
		//	m_animation->AddComponent(std::make_shared<Animation::C_ElipseTranslateAnim>(1.0f, 1.0f));
		//}
		// ----------- just for test -------------

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

		auto& shdManager = C_ShaderManager::Instance();
		m_program = shdManager.GetProgram("basic-planes");
		m_shadowProgram = shdManager.GetProgram("basic-shadow");
	}

	//=================================================================================
	bool C_MeshNode::IsAABBChanged() const
	{
		if (m_animation->GetTRSMatrix() != glm::mat4(1.0f)) {
			return true;
		}
		return T_Base::IsAABBChanged();
	}

	//=================================================================================
	void C_MeshNode::ShadowPass(const S_RenderParams& params) const
	{
		m_shadowProgram->useProgram();

		ErrorCheck();
		glBindVertexArray(m_VAO);
		// bind VBOs for vertex array and index array
		glEnableVertexAttribArray(0);
		ErrorCheck();

		glUniformMatrix4fv(glGetUniformLocation(m_shadowProgram->GetProgram(), "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(params.m_cameraViewProjectionMatrix));
		glUniformMatrix4fv(glGetUniformLocation(m_shadowProgram->GetProgram(), "modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_modelMatrix * m_animation->GetTRSMatrix()));
		ErrorCheck();

		glDrawArrays(GL_TRIANGLES, 0, m_triangles);
		ErrorCheck();

		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_shadowProgram->disableProgram();
		ErrorCheck();
	}

	//=================================================================================
	void C_MeshNode::RenderPass(const S_RenderParams& params) const
	{
		RenderBBox(params.m_cameraViewProjectionMatrix);


		glBindVertexArray(m_VAO);
		m_program->useProgram();

		// bind VBOs for vertex array and index array
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		//glEnableVertexAttribArray(2);


		glActiveTexture(GL_TEXTURE0);
		params.m_shadowMap->bind();
		glUniform1i(glGetUniformLocation(m_program->GetProgram(), "shadowMap"), 0);
		glm::vec4 light(0, 15.0f, 0.f, 1.0f);
		glUniform4fv(glGetUniformLocation(m_program->GetProgram(), "lightPos"), 1, glm::value_ptr(light));
		glUniform4fv(glGetUniformLocation(m_program->GetProgram(), "PSSMLimits"), 1, glm::value_ptr(params.m_planes));
		glUniform4fv(glGetUniformLocation(m_program->GetProgram(), "CameraPosition"), 1, glm::value_ptr(Application::Instance().GetCamManager()->GetMainCamera()->getPosition()));

		glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "toShadowMapSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(params.m_toShadowMapSpaceMatrix));
		glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(params.m_cameraViewProjectionMatrix));
		glUniformMatrix4fv(glGetUniformLocation(m_program->GetProgram(), "modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_modelMatrix * m_animation->GetTRSMatrix()));

		glDrawArrays(GL_TRIANGLES, 0, m_triangles);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		//glDisableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		m_program->disableProgram();
		params.m_shadowMap->unbind();
	}

	//=================================================================================
	void C_MeshNode::RenderBBox(const glm::mat4& projectionMatrix) const
	{
		C_DebugDraw::Instance().DrawAABB(m_bbox, m_modelMatrix * m_animation->GetTRSMatrix(), projectionMatrix);
	}
};
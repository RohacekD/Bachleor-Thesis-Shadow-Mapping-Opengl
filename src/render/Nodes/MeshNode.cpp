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

		glGenBuffers(1, &m_VBOs[0]);
		glGenBuffers(1, &m_VBOs[1]);
		glGenBuffers(1, &m_VBOs[2]);
	}

	//=================================================================================
	C_MeshNode::~C_MeshNode()
	{
		DestructorFullCheck();
	}

	//=================================================================================
	void C_MeshNode::RenderMyself(const S_RenderParams& params, const glm::mat4& modelMatrix)
	{
		if (params.m_pass == render::S_RenderParams::E_PassType::RenderPass) {
			RenderPass(params, modelMatrix);
		}
		else if (params.m_pass == render::S_RenderParams::E_PassType::ZPass) {
			ZPass(params, modelMatrix);
		}
		else {
			ShadowPass(params, modelMatrix);
		}
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
		DestructorFullCheck();
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


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

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
	void C_MeshNode::ShadowPass(const S_RenderParams& params, const glm::mat4& modelMatrix) const
	{
		ErrorCheck();
		m_shadowProgram->useProgram();

		glBindVertexArray(m_VAO);

		m_shadowProgram->SetUniform("modelMatrix", modelMatrix * m_animation->GetTRSMatrix());

		glDrawArrays(GL_TRIANGLES, 0, m_triangles);
		ErrorCheck();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_shadowProgram->disableProgram();
		ErrorCheck();
	}

	//=================================================================================
	void C_MeshNode::RenderPass(const S_RenderParams& params, const glm::mat4& modelMatrix) const
	{
		//RenderBBox(params.m_cameraViewProjectionMatrix, modelMatrix);


		m_program->useProgram();
		glBindVertexArray(m_VAO);

		glActiveTexture(GL_TEXTURE0);
		params.m_shadowMap->bind();
		m_program->SetUniform("shadowMap", 0);

		glActiveTexture(GL_TEXTURE1);
		if (m_texture) {
			m_texture->bind();
			m_program->SetUniform("tex", 1);
		}

		m_program->SetUniform("projectionMatrix", params.m_cameraViewProjectionMatrix);
		m_program->SetUniform("modelMatrix", modelMatrix * m_animation->GetTRSMatrix());

		glDrawArrays(GL_TRIANGLES, 0, m_triangles);

		glBindVertexArray(0);
		m_program->disableProgram();
		
		params.m_shadowMap->unbind();
		if (m_texture)
		{
			m_texture->unbind();
		}
	}

	//=================================================================================
	void C_MeshNode::ZPass(const S_RenderParams& params, const glm::mat4& modelMatrix) const
	{
		auto& shdManager = C_ShaderManager::Instance();
		ErrorCheck();
		auto program = shdManager.GetProgram("depthsamples");
		program->useProgram();

		glBindVertexArray(m_VAO);

		program->SetUniform("modelMatrix", modelMatrix * m_animation->GetTRSMatrix());
		program->SetUniform("projectionMatrix", params.m_cameraViewProjectionMatrix);

		glDrawArrays(GL_TRIANGLES, 0, m_triangles);
		ErrorCheck();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		program->disableProgram();
		ErrorCheck();
	}

	//=================================================================================
	void C_MeshNode::RenderBBox(const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) const
	{
		C_DebugDraw::Instance().DrawAABB(m_bbox, projectionMatrix, glm::vec3(0.0f), modelMatrix * m_animation->GetTRSMatrix());
	}
};
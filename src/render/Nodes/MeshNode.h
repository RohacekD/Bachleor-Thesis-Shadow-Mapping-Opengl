/**
 * ==============================================
 * \file 		MeshNode.h
 * \date 		2018/03/17 19:01
 * \project 	Computer Graphics Project
 * \faculty 	Faculty of Information Technology 
 * \university 	Brno University of Technology
 *
 * \author Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
*/

#pragma once

#include "RenderNode.h"
#include "GLW/Texture.h"

#include <GL/glew.h>

#include <memory>

struct Mesh;

namespace GLW {
	class C_ShaderProgram;
}

namespace render {
	class C_MeshNode : public I_RenderNode {
		using T_Base = I_RenderNode;
	public:
		C_MeshNode();
		virtual ~C_MeshNode();

		
		//=================================================================================
		virtual void RenderMyself(const S_RenderParams& params, const glm::mat4& modelMatrix) override;

		virtual void Clean() override;

		void LoadMesh(const Mesh& mesh);
		void SetTexture(std::shared_ptr<GLW::C_Texture> texture) { m_texture = texture; }
		void SetColor(const glm::vec4& color) noexcept { m_color = color; }


		virtual bool IsAABBChanged() const override;

	private:
		void ShadowPass(const S_RenderParams& params, const glm::mat4& modelMatrix) const;
		void RenderPass(const S_RenderParams& params, const glm::mat4& modelMatrix) const;
		void ZPass(const S_RenderParams& params, const glm::mat4& modelMatrix) const;

		void RenderBBox(const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) const;

		GLuint m_triangles;
		GLuint m_VBOs[3];
		GLuint m_VAO;
		std::shared_ptr<GLW::C_ShaderProgram> m_program;
		std::shared_ptr<GLW::C_ShaderProgram> m_shadowProgram;
		std::shared_ptr<GLW::C_Texture> m_texture;

		glm::vec4 m_color;
	};
};
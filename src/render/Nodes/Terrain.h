#pragma once

#include "render/Nodes/RenderNode.h"

struct Texture;

namespace GLW {
	class C_ShaderProgram;
}

namespace render {

	class C_Terrain : public I_RenderNode
	{
		using T_Base = I_RenderNode;
	public:
		C_Terrain(float tileSize, const Texture& texture);
		virtual ~C_Terrain();

		virtual void Render(const S_RenderParams& params) override;
		virtual void Clean() override;
		virtual void Update(const double timeSinceLastUpdateMs) override;
		virtual void PostUpdate() override;
		virtual bool IsAABBChanged() const override;
		virtual const glm::mat4 GetActualModelMatrix() const override;

	private:
		unsigned int m_x = 0;
		unsigned int m_z = 0;
		float m_tileSize = 1.0f;
		std::size_t m_indicesNum = 0;

		GLuint m_VBOs[3];
		GLuint m_VAO;
		GLuint m_elementbuffer;
		std::shared_ptr<GLW::C_ShaderProgram> m_program;

	};
}
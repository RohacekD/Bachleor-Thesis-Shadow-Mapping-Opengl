#pragma once

#include "RenderNode.h"
#include "Texture.h"

#include <GL/glew.h>

#include <memory>

struct Mesh;

namespace render {
	class C_MeshNode : public I_RenderNode {
		using T_Base = I_RenderNode;
	public:
		C_MeshNode();
		virtual ~C_MeshNode();

		virtual void Render(const S_RenderParams& params) override;

		virtual void Clean() override;

		void LoadMesh(const Mesh& mesh);
		void SetTexture(std::shared_ptr<GL::C_Texture> texture) { m_texture = texture; }

	private:
		GLuint m_triangles;
		GLuint m_VBOs[3];
		GLuint m_VAO;
		GLuint m_program;
		std::shared_ptr<GL::C_Texture> m_texture;
	};
};
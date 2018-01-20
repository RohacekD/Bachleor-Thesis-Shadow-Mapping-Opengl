#pragma once

#include <gl/glew.h>

#include <map>
#include <memory>


namespace GLW {
	class C_Texture;
	class C_Framebuffer {
	public:
		C_Framebuffer();
		~C_Framebuffer();

		void Bind();
		void Unbind();

		void AttachTexture(GLenum attachement, std::shared_ptr<C_Texture> texture);
		std::shared_ptr<C_Texture> GetAttachement(GLenum attachement);

	private:
		GLuint m_FBO;
		std::map<GLenum, std::shared_ptr<C_Texture>> m_attachements;
	};
}
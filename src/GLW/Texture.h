#pragma once

#include <GL/glew.h>

#include <string>

namespace GLW {
	//wrapper for opengl texture
	// for now I only going to work with GL_TEXTURE_2D
	class C_Texture {
	public:
		C_Texture(GLenum target = GL_TEXTURE_2D);
		C_Texture(const std::string& name, GLenum target = GL_TEXTURE_2D);
		~C_Texture();

		void bind() const;
		void unbind() const;

		inline void StartGroupOp() { bind(); m_bGroupOperations = true;  }
		inline void EndGroupOp() { m_bGroupOperations = false; unbind(); }

		//just for now
		inline GLuint GetTexture() const { return m_texture; }
		inline GLenum GetTarget() const { return m_target; }

		void setWrap(GLint wrapS, GLint wrapT);
		void setFilter(GLint min, GLint mag);
		void setParameteri(GLenum pname, GLint param);

	private:
		GLuint m_texture;
		GLenum m_target;
		bool m_bGroupOperations : 1;
	};
};
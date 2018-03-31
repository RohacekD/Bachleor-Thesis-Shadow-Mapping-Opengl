/**
 * ==============================================
 * \file 		Texture.h
 * \date 		2018/03/17 19:07
 * \project 	Computer Graphics Project
 * \faculty 	Faculty of Information Technology 
 * \university 	Brno University of Technology
 *
 * \author Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
 */

#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
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

		void SetWrap(GLint wrapS, GLint wrapT);
		void SetFilter(GLint min, GLint mag);
		void SetTexParameter(GLenum pname, const glm::vec4& value);
		void SetTexParameter(GLenum pname, GLint value);

	private:
		GLuint m_texture;
		GLenum m_target;
		bool m_bGroupOperations : 1;
	};
};
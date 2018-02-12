#include "Texture.h"
#include "Debug.h"

namespace GLW {

	//=================================================================================
	C_Texture::C_Texture(GLenum target)
		: m_bGroupOperations(false)
		, m_target(target)
	{
		glGenTextures(1, &m_texture);
		ErrorCheck();
	}

	//=================================================================================
	C_Texture::C_Texture(const std::string & name, GLenum target)
		:C_Texture(target)
	{
		bind();
		glObjectLabel(GL_TEXTURE, m_texture, static_cast<GLsizei>(name.length()), name.c_str());
		ErrorCheck();
		unbind();
	}

	//=================================================================================
	C_Texture::~C_Texture()
	{
		ErrorCheck();
		glDeleteTextures(1, &m_texture);
		ErrorCheck();
	}

	//=================================================================================
	void C_Texture::bind() const
	{
		ErrorCheck();
		if (!m_bGroupOperations)
			glBindTexture(m_target, m_texture);
		ErrorCheck();
	}

	//=================================================================================
	void C_Texture::unbind() const
	{
		if (!m_bGroupOperations)
			glBindTexture(m_target, 0);
	}

	//=================================================================================
	void C_Texture::setWrap(GLint wrapS, GLint wrapT)
	{
		bind();
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, wrapT);
		unbind();
	}

	//=================================================================================
	void C_Texture::setFilter(GLint min, GLint mag)
	{
		bind();
		glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, min);
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, mag);
		unbind();
	}
};
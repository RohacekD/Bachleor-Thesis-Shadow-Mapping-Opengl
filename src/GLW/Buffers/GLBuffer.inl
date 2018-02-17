#pragma once

#include "Debug.h"

namespace GLW {
	//=================================================================================
	template<GLenum TYPE>
	C_GLBuffer<TYPE>::C_GLBuffer()
	{
		glGenBuffers(1, &m_id);
	}

	//=================================================================================
	template<GLenum TYPE>
	C_GLBuffer<TYPE>::~C_GLBuffer()
	{
		glDeleteBuffers(1, &m_id);
		DestructorFullCheck();
	}

	//=================================================================================
	template<GLenum TYPE>
	void C_GLBuffer<TYPE>::unbind() const
	{
		glBindBuffer(TYPE, m_id);
	}

	//=================================================================================
	template<GLenum TYPE>
	void C_GLBuffer<TYPE>::bind() const
	{
		glBindBuffer(TYPE, 0);
	}
}
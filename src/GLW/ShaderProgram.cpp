#include "GLW/ShaderProgram.h"

#include "Debug.h"

namespace GLW {

	//=================================================================================
	C_ShaderProgram::C_ShaderProgram(GLuint program)
		: m_Program(program)
	{}

	//=================================================================================
	C_ShaderProgram::~C_ShaderProgram()
	{
		ErrorCheck();
		glDeleteProgram(m_Program);
		ErrorCheck();
	}

	//=================================================================================
	void C_ShaderProgram::useProgram() const
	{
		glUseProgram(m_Program);
	}

	//=================================================================================
	void C_ShaderProgram::disableProgram() const
	{
		glUseProgram(0);
	}
}
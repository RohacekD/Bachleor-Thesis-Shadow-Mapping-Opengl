#include "GLW/ShaderProgram.h"

namespace GLW {

	//=================================================================================
	C_ShaderProgram::C_ShaderProgram(GLuint program)
		: m_Program(program)
	{}

	//=================================================================================
	C_ShaderProgram::~C_ShaderProgram()
	{
		glDeleteProgram(m_Program);
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
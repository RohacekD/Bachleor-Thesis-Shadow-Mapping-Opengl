#include "GLW/ShaderProgram.h"

#include "GLW/Buffers/UniformBuffer.h"

#include "Debug.h"

namespace GLW {

	//=================================================================================
	C_ShaderProgram::C_ShaderProgram(GLuint program)
		: m_Program(program)
	{}

	//=================================================================================
	C_ShaderProgram::~C_ShaderProgram()
	{
		glDeleteProgram(m_Program);
		DestructorFullCheck();
	}

	//=================================================================================
	void C_ShaderProgram::useProgram()
	{
		glUseProgram(m_Program);
		m_bIsActive = true;
	}

	//=================================================================================
	void C_ShaderProgram::disableProgram()
{
		glUseProgram(0);
		m_bIsActive = false;
	}

	//=================================================================================
	void C_ShaderProgram::BindUBO(std::shared_ptr<C_UniformBuffer> ubo) const
	{
		int uboBlockLocation = FindUniformBlockLocation(ubo->GetBlockName().c_str());
		if (uboBlockLocation > 0) {
			glUniformBlockBinding(m_Program, uboBlockLocation, ubo->GetBinding());
		}
	}

}
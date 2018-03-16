#include "GLW/ShaderProgram.h"

#include "GLW/Buffers/UniformBuffer.h"

#include "Debug.h"

namespace GLW {

	//=================================================================================
	C_ShaderProgram::C_ShaderProgram(GLuint program)
		: m_Program(program)
	{}

	//=================================================================================
	C_ShaderProgram::C_ShaderProgram(C_ShaderProgram&& rhs)
		: m_Program(rhs.m_Program)
		, m_uniformMap(std::move(rhs.m_uniformMap))
	{
		std::cout << "Move semantics" << std::endl;
	}

	//=================================================================================
	GLW::C_ShaderProgram& C_ShaderProgram::operator=(C_ShaderProgram&& other)
	{
		if (this != &other) {
			m_Program = other.m_Program;
			m_uniformMap = std::move(other.m_uniformMap);
		}
		return *this;
	}

	//=================================================================================
	C_ShaderProgram::~C_ShaderProgram()
	{
		glDeleteProgram(m_Program);
		DestructorFullCheck();
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

	//=================================================================================
	void C_ShaderProgram::BindUBO(std::shared_ptr<C_UniformBuffer> ubo) const
	{
		int uboBlockLocation = FindUniformBlockLocation(ubo->GetBlockName().c_str());
		if (uboBlockLocation > 0) {
			glUniformBlockBinding(m_Program, uboBlockLocation, ubo->GetBinding());
		}
	}

}
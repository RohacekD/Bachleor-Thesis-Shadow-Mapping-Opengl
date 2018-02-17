#pragma once 
#if _DEBUG
#include <iostream>
#endif

namespace GLW {

	//=================================================================================
	template<>
	inline int C_ShaderProgram::FindLocation(const char* name) const
	{
		GLint location = glGetUniformLocation(m_Program, name);
#if _DEBUG
		if (location < 0) {
			std::cerr << "Program number #" << m_Program << " doesn't have uniform: " << name << std::endl;
		}
#endif
		return location;
	}

	//=================================================================================
	template<>
	inline int C_ShaderProgram::FindLocation(const std::string& name) const
	{
		GLint location = glGetUniformLocation(m_Program, name.c_str());
#if _DEBUG
		if (location < 0) {
			std::cerr << "Program number #" << m_Program << " doesn't have uniform: " << name << std::endl;
		}
#endif
		return location;
	}

	//=================================================================================
	template<class N>
	void C_ShaderProgram::SetUniform(N name, const int & value)
	{
		glUniform1i(FindLocation(name), value);
	}

	//=================================================================================
	template<class N>
	void GLW::C_ShaderProgram::SetUniform(N name, const glm::mat4 & value)
	{
		glUniformMatrix4fv(FindLocation(name), 1, GL_FALSE, glm::value_ptr(value)); 
		ErrorCheck();
	}

	//=================================================================================
	template<class N>
	void GLW::C_ShaderProgram::SetUniform(N name, const glm::vec4 & value)
	{
		glUniform4fv(FindLocation(name), 1, glm::value_ptr(value));
		ErrorCheck();
	}

	//=================================================================================
	template<class N>
	void GLW::C_ShaderProgram::SetUniform(N name, const glm::vec3 & value)
	{
		glUniform3fv(FindLocation(name), 1, glm::value_ptr(value));
		ErrorCheck();
	}

	//=================================================================================
	template<class N>
	void GLW::C_ShaderProgram::SetUniform(N name, const std::vector<float> & value)
	{
		glUniform1fv(FindLocation(name), static_cast<GLsizei>(value.size()), (GLfloat*)(value.data()));
	}

	//=================================================================================
	template<class N>
	void GLW::C_ShaderProgram::SetUniform(N name, const std::vector<int> & value)
	{
		glUniform1iv(FindLocation(name), static_cast<GLsizei>(value.size()), (value.data()));
	}

	//=================================================================================
	template<class N>
	void GLW::C_ShaderProgram::SetUniform(N name, const float & value)
	{
		glUniform1f(FindLocation(name), static_cast<GLfloat>(value));
	}
}
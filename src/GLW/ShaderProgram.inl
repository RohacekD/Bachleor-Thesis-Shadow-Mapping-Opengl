#pragma once 

namespace GLW {

	//=================================================================================
	template<>
	inline int C_ShaderProgram::FindLocation(const char* name) const
	{
		return glGetUniformLocation(m_Program, name);
	}

	//=================================================================================
	template<>
	inline int C_ShaderProgram::FindLocation(const std::string& name) const
	{
		return glGetUniformLocation(m_Program, name.c_str());
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
}
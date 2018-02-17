#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>
#include <vector>

namespace GLW {
	class C_ShaderProgram {
	public:
		C_ShaderProgram(GLuint program);
		virtual ~C_ShaderProgram();
		void useProgram() const;
		void disableProgram() const;

		// replace this
		inline GLuint GetProgram() const { return m_Program; }

		template<class N, class T> void SetUniform(N name, T value);
		template<class N> void SetUniform(N name, const int & value);
		template<class N> void SetUniform(N name, const float & value);
		template<class N> void SetUniform(N name, const glm::mat4 & value);
		template<class N> void SetUniform(N name, const glm::vec4 & value);
		template<class N> void SetUniform(N name, const glm::vec3 & value);
		template<class N> void SetUniform(N name, const std::vector<int> & value);
		template<class N> void SetUniform(N name, const std::vector<float> & value);

		template<class T> int FindLocation(T name);
		template<> int FindLocation(const char* name);
		template<> int FindLocation(const std::string& name);

	private:
		GLuint m_Program;
		std::map<std::string, GLint> m_uniformMap;
	};
}
#include "GLW/ShaderProgram.inl"
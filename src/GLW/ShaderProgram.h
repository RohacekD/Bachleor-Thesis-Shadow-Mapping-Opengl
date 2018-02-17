#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

		template<class T> int FindLocation(T name) const;
		template<> int FindLocation(const char* name) const;
		template<> int FindLocation(const std::string& name) const;

	private:
		GLuint m_Program;
	};
}
#include "GLW/ShaderProgram.inl"
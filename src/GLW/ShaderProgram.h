/**
 * ==============================================
 * \file 		ShaderProgram.h
 * \date 		2018/03/17 19:07
 * \project 	Computer Graphics Project
 * \faculty 	Faculty of Information Technology 
 * \university 	Brno University of Technology
 *
 * \author Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
 */

#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace GLW {
	class C_UniformBuffer;

	class C_ShaderProgram {
	public:
		C_ShaderProgram(GLuint program);
#if _DEBUG
		//C_ShaderProgram(GLuint program);
#endif
		C_ShaderProgram(C_ShaderProgram&& rhs);
		C_ShaderProgram& operator=(C_ShaderProgram&& other);
		C_ShaderProgram& operator=(C_ShaderProgram& other) = delete;
		C_ShaderProgram(C_ShaderProgram& rhs) = delete;
		virtual ~C_ShaderProgram();

		void useProgram() const;
		void disableProgram() const;

		void BindUBO(std::shared_ptr<C_UniformBuffer>) const;


		// replace this
		inline GLuint GetProgram() const { return m_Program; }

		template<class N, class T> void SetUniform(N name, T value);
		template<class N> void SetUniform(N name, const int & value);
		template<class N> void SetUniform(N name, const float & value);
		template<class N> void SetUniform(N name, const glm::mat4 & value);
		template<class N> void SetUniform(N name, const glm::vec4 & value);
		template<class N> void SetUniform(N name, const glm::vec3 & value);
		template<class N> void SetUniform(N name, const glm::ivec2 & value);
		template<class N> void SetUniform(N name, const std::vector<int> & value);
		template<class N> void SetUniform(N name, const std::vector<float> & value);

		template<class T> int FindLocation(T name);
		template<> int FindLocation(const char* name);
		template<> int FindLocation(const std::string& name);

		template<class T> int FindUniformBlockLocation(T name) const;
		template<> int FindUniformBlockLocation(const char* name) const;
		template<> int FindUniformBlockLocation(const std::string& name) const;

	private:
		GLuint m_Program;
		std::map<std::string, GLint> m_uniformMap;
	};
}
#include "GLW/ShaderProgram.inl"
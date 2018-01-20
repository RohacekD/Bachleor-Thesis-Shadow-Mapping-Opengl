#pragma once

#include <gl/glew.h>


namespace GLW {
	class C_ShaderProgram {
	public:
		C_ShaderProgram(GLuint program);
		virtual ~C_ShaderProgram();
		void useProgram() const;
		void disableProgram() const;

		// replace this
		inline GLuint GetProgram() const { return m_Program; }
	private:
		GLuint m_Program;
	};
}
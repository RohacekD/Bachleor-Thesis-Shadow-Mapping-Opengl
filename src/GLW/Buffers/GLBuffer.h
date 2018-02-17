#pragma once

#include <GL/glew.h>

#include <string>

namespace GLW {
	template<GLenum TYPE>
	class C_GLBuffer {
	public:
		C_GLBuffer();
		virtual ~C_GLBuffer();

		void bind() const;
		void unbind() const;
	protected:
		GLuint m_id;
	};
}
#include "GLW/Buffers/GLBuffer.inl"
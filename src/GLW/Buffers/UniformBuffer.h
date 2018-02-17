#pragma once

#include "GLW/Buffers/GLBuffer.h"

#include <GL/glew.h>

#include <string>

namespace GLW {
	class C_UniformBuffer : public C_GLBuffer<GL_UNIFORM_BUFFER> {
	public:
		C_UniformBuffer(const std::string& blockName);
		virtual ~C_UniformBuffer();
	private:
		GLuint m_location;
		std::string m_blockName;
	};
}
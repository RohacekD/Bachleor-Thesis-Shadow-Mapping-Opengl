/**
 * ==============================================
 * \file 		UniformBuffer.h
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

#include "GLW/Buffers/GLBuffer.h"

#include <GL/glew.h>

#include <string>

namespace GLW {
	class C_UniformBuffer : public C_GLBuffer<GL_UNIFORM_BUFFER> {
	public:
		C_UniformBuffer(const std::string& blockName, unsigned int index);
		virtual ~C_UniformBuffer();

		virtual void bind() const override;

		virtual void UploadData() const = 0;

		void Activate(bool activate = true);
		inline bool IsActive() const { return m_active; }

		const std::string& GetBlockName() const { return m_blockName; }
		int GetBinding() const { return m_index; }
	private:
		GLuint m_index;
		std::string m_blockName;
		bool m_active;
	};
}
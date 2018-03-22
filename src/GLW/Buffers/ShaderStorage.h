/** ==============================================
 * @file 		ShaderStorage.h
 * @date 		2018/03/18 11:01
 * @project 	Opengl Shadow Rendering
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include "GLW/Buffers/GLBuffer.h"

namespace GLW {
	class C_ShaderStorageBuffer : public C_GLBuffer<GL_SHADER_STORAGE_BUFFER> {
	public:
		C_ShaderStorageBuffer(int bindingPoint);
		virtual void UploadData() const = 0;
		virtual void DownloadData() = 0;
		virtual void ClearBuffer() = 0;

		virtual void bind() const override;
		inline void SetBindingPoint(GLuint binding) noexcept { m_BindingPoint = binding; }
	private:
		GLuint m_BindingPoint;
	};
}

//for now it can stay here
class C_Histogram : public GLW::C_ShaderStorageBuffer {
public:
	C_Histogram(unsigned int samples, int bindingPoint);

	virtual void UploadData() const override;
	virtual void DownloadData() override;

	glm::uvec2 FindLimits() const;
	int			GetHighest() const;

	std::vector<unsigned int>	m_histogram;
	unsigned int				m_highestValue;
	unsigned int				m_lowestIndex;
	unsigned int				m_highestIndex;

	virtual void ClearBuffer() override;

private:
	unsigned int m_samples;
};
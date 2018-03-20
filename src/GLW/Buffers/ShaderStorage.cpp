#include "ShaderStorage.h"
#include <algorithm>    // std::min_element, std::max_element

namespace GLW {

	//=================================================================================
	C_ShaderStorageBuffer::C_ShaderStorageBuffer(int bindingPoint)
		: m_BindingPoint(bindingPoint)
	{

	}

	//=================================================================================
	void C_ShaderStorageBuffer::bind() const
	{
		C_GLBuffer<GL_SHADER_STORAGE_BUFFER>::bind();
		glBindBufferBase(GetBufferType(), m_BindingPoint, m_id);
	}

}

//=================================================================================
C_Histogram::C_Histogram(int samples, int bindingPoint)
	: m_samples(samples)
	, C_ShaderStorageBuffer(bindingPoint)
{
	ErrorCheck();
	bind();
	glBufferData(GetBufferType(), (m_samples + 1)*sizeof(int), nullptr, GL_DYNAMIC_DRAW);
	ErrorCheck();
}

//=================================================================================
void C_Histogram::UploadData() const
{
	bind();
	unsigned int* data = (unsigned int *)glMapBuffer(GetBufferType(), GL_READ_WRITE);

	std::vector<unsigned int> v = { 257, 0 };

	data = v.data();

	glUnmapBuffer(GetBufferType());
}

//=================================================================================
void C_Histogram::DownloadData()
{
	bind();
	int* data = (int *)glMapBuffer(GetBufferType(), GL_READ_WRITE);

	m_histogram = std::vector<unsigned int>(data, data+m_samples);
	m_highestValue = data[m_samples];

	glUnmapBuffer(GetBufferType());
}

//=================================================================================
glm::ivec2 C_Histogram::FindLimits() const
{
	glm::ivec2 ret(m_samples, 0);
	for (int i = 0; i < m_samples; ++i) {
		const int sample = m_histogram[i];
		if (sample == 0) continue;
		if (i < ret.x) {
			ret.x = i;
		}
		if (i > ret.y) {
			ret.y = i;
		}
	}
	return ret;
}

//=================================================================================
int C_Histogram::GetHighest() const
{
	return *std::max_element(m_histogram.cbegin(), m_histogram.cend());
}

//=================================================================================
void C_Histogram::ClearBuffer()
{
	glClearBufferData(GetBufferType(), GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr );
}

#include "PSSMUBO.h"

//=================================================================================
C_PSSMUBO::C_PSSMUBO(const std::string& blockName, unsigned int index, unsigned int splits)
	: GLW::C_UniformBuffer(blockName, index)
	, m_splits(splits)
{
	const auto matSize = sizeof(glm::mat4);
	m_lightViewProjections.resize(m_splits);

	const auto bytes = 2 * matSize + matSize * m_splits + s_sunDirectionSize;

	ErrorCheck();
	GLW::C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, bytes, nullptr, GL_DYNAMIC_DRAW);
	ErrorCheck();
}

//=================================================================================
void C_PSSMUBO::UploadData() const
{
	bind();
	char *data = (char *)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);

	const auto matSize = sizeof(decltype(m_cameraView));

	memcpy(data, glm::value_ptr(m_cameraView), matSize);
	memcpy(data + matSize, glm::value_ptr(m_cameraProjection), matSize);
	memcpy(data + (2 * matSize), m_lightViewProjections.data(), matSize*m_splits);
	memcpy(data + (2 * matSize) + matSize * m_splits, glm::value_ptr(m_SunDirection), s_sunDirectionSize);

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	unbind();
}

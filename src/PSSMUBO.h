#pragma once

#include <gl/glew.h>

#include "GLW/Buffers/UniformBuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <vector>

template<int SPLITS>
class C_PSSMUBO : public GLW::C_UniformBuffer {
public:
	C_PSSMUBO(const std::string& blockName, unsigned int index);

	unsigned int m_splits;
	glm::mat4 m_cameraView; // view projection of bound camera
	glm::mat4 m_cameraProjection;
	std::vector<float> m_splitPlanes;
	std::array<glm::mat4, SPLITS> m_lightViewProjections;


	virtual void UploadData() const override;

};

//=================================================================================
template<int SPLITS>
C_PSSMUBO<SPLITS>::C_PSSMUBO(const std::string& blockName, unsigned int index)
	: GLW::C_UniformBuffer(blockName, index)
{
	const auto matSize = sizeof(glm::mat4);
	const auto splitPlanesSize = sizeof(float) * SPLITS;

	const auto bytes = 2*matSize + splitPlanesSize + matSize * SPLITS;

	ErrorCheck();
	GLW::C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, bytes, nullptr, GL_DYNAMIC_DRAW);
	ErrorCheck();
}

//=================================================================================
template<int SPLITS>
void C_PSSMUBO<SPLITS>::UploadData() const
{
	bind();
	char *data = (char *)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);

	const auto matSize = sizeof(glm::mat4);
	const auto splitPlanesSize = sizeof(float);

	memcpy(data, m_splitPlanes.data(), splitPlanesSize * m_splits);
	memcpy(data + splitPlanesSize * m_splits, glm::value_ptr(m_cameraView), matSize);
	memcpy(data + splitPlanesSize * m_splits + matSize, glm::value_ptr(m_cameraProjection), matSize);
	memcpy(data + splitPlanesSize * m_splits + (2 * matSize), m_lightViewProjections.data(), matSize*SPLITS);

	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

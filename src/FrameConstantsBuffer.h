#pragma once

#include "GLW\Buffers\UniformBuffer.h"

#include <glm\detail\type_mat.hpp>
#include <glm\detail\type_vec.hpp>

class C_FrameConstantsBuffer : public GLW::C_UniformBuffer {
public:
	C_FrameConstantsBuffer(const std::string& blockName, unsigned int index);

	virtual void UploadData() const override;

	const glm::vec4& GetCameraPosition() const { return m_CameraPosition; }
	void SetCameraPosition(const glm::vec4& val) { m_CameraPosition = val; }
	const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
	void SetViewProjection(const glm::mat4& val) { m_ViewProjection = val; }
private:
	glm::vec4 m_CameraPosition;
	glm::mat4 m_ViewProjection;
};
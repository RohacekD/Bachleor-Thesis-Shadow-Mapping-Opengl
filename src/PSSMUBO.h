/** ==============================================
 * @file 		PSSMUBO.h
 * @date 		2018/03/17 19:54
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include <gl/glew.h>

#include "GLW/Buffers/UniformBuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <vector>

// @todo get rid of template, have no meaning here

class C_PSSMUBO : public GLW::C_UniformBuffer {
public:
	C_PSSMUBO(const std::string& blockName, unsigned int index, unsigned int splits);

	glm::mat4 m_cameraView; // view projection of bound camera
	glm::mat4 m_cameraProjection;
	std::vector<glm::mat4> m_lightViewProjections;
	glm::vec3	m_SunDirection;


	virtual void UploadData() const override;
	void SetNumSplits();
	unsigned int GetNumSplits() const { return m_splits; }

private:
	unsigned int m_splits;
	static const auto s_sunDirectionSize = sizeof(glm::vec3);
};
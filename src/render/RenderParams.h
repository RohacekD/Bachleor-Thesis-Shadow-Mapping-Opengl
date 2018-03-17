#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "GLW/Texture.h"

namespace render {
	struct S_RenderParams {
		enum class E_PassType : char {
			RenderPass,
			ShadowPass,
			ZPass
		};
		glm::mat4 m_cameraViewProjectionMatrix;
		glm::vec3 m_cameraPosition;
		std::shared_ptr<GLW::C_Texture> m_shadowMap;
		E_PassType m_pass;
		std::vector<float>	m_planes;

	};
}
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "GLW/Texture.h"

namespace render {
	struct S_RenderParams {
		enum E_PassType : char {
			E_P_RenderPass,
			E_P_ShadowPass
		};
		glm::mat4 m_cameraViewProjectionMatrix;
		glm::vec3 m_cameraPosition;
		glm::mat4 m_toShadowMapSpaceMatrix;
		std::shared_ptr<GLW::C_Texture> m_shadowMap;
		E_PassType m_pass;
		std::vector<float>	m_planes;

	};
}
/**
 * ==============================================
 * \file 		RenderParams.h
 * \date 		2018/03/17 19:08
 * \project 	Computer Graphics Project
 * \faculty 	Faculty of Information Technology 
 * \university 	Brno University of Technology
 *
 * \author Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
 */
 
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "GLW/Texture.h"
#include "glm/detail/type_mat.hpp"

namespace render {
	/**
	 * ==============================================
	 * \class S_RenderParams
	 *
	 * \brief	This class is used to pass information about current 
	 *			render pass through scene tree
	 *
	 * \author Dominik Rohacek
	 * Contact: RohacekD@gmail.com
	 * \date 2018/03/17
	 * ==============================================
	 */
	struct S_RenderParams {
		/**
		 *
		 * \brief	This enum determine type of render pass
		 */
		enum class E_PassType : char {
			RenderPass,	//< Final render pass
			ShadowPass, //< Just silhouette is needed during shadow pass
			ZPass		//< This pass is used only to get depth of shadow casters depths
		};
		glm::mat4 m_cameraViewProjectionMatrix; //< Pre-multiplicated view projection matrix
		glm::vec3 m_cameraPosition;
		// @todo: there should be some mechanism to control that texture is GL_TEXTURE_ARRAY_2D
		std::shared_ptr<GLW::C_Texture> m_shadowMap; //< Contains shadow map cascade map
		E_PassType m_pass;				
		std::vector<float>	m_planes;	//< Splitting planes of shadow cascade

	};
}
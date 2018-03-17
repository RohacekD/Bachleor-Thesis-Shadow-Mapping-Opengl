/**
 * ==============================================
 * \file 		Scene.h
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

#include "RenderNode.h"
#include <glm/glm.hpp>

namespace render {
	// Scene is special node with no actual geometry assigned
	// it should be used as root node
	class C_Scene : public I_RenderNode {
		using T_Base = I_RenderNode;
	public:
		C_Scene();
		C_Scene(const glm::vec3& position);

		virtual void PostUpdate() override;


		virtual void RenderMyself(const S_RenderParams& params, const glm::mat4& modelMatrix) override;

	};

};
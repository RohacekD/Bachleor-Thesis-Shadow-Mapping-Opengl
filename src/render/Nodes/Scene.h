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
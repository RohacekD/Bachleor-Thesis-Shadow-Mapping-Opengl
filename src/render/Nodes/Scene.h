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

		virtual void Render(const S_RenderParams& params) override;


		virtual void PostUpdate() override;

	};

};
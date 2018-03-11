#pragma once

#include "render/Nodes/RenderNode.h"

namespace render {

	class C_Terrain : public I_RenderNode
	{
	public:
		C_Terrain();
		virtual ~C_Terrain();


		virtual void Render(const S_RenderParams& params) override;


		virtual void Clean() override;


		virtual void Update(const double timeSinceLastUpdateMs) override;


		virtual void PostUpdate() override;


		virtual bool IsAABBChanged() const override;


		virtual const glm::mat4 GetActualModelMatrix() const override;

	private:

	};
}
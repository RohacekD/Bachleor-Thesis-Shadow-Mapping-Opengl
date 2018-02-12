#include "render/Nodes/Scene.h"

#include "render/RenderParams.h"

#include "Debug.h"

namespace render {

	//=================================================================================
	C_Scene::C_Scene()
		: I_RenderNode({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f })
	{
	}

	//=================================================================================
	void C_Scene::Render(const S_RenderParams& params)
	{
		//if (params.m_pass == render::S_RenderParams::E_PassType::E_P_RenderPass) {
		//	C_DebugDraw::Instance().DrawAABB(m_bbox, params.m_cameraViewProjectionMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
		//}

		T_Base::Render(params);
	}

	//=================================================================================
	void C_Scene::PostUpdate()
	{
		T_Base::PostUpdate();

		m_bbox = AABB();
		for (const auto & node : *this) {
			m_bbox.updateWithAABB(node->GetActualAABB());
		}
	}

};
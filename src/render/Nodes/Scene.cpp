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
	C_Scene::C_Scene(const glm::vec3& position)
		: I_RenderNode(position, { 0.0f, 0.0f, 0.0f })
	{

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

	//=================================================================================
	void C_Scene::RenderMyself(const S_RenderParams& params, const glm::mat4& modelMatrix)
	{
		//this is just a wrapper
	}

};
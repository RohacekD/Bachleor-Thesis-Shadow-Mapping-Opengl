#include "RenderNode.h"
#include "render/RenderParams.h"
#include "animation/Animation.h"
#include "GLW/ShaderProgram.h"
#include "Debug.h"
#include "ShaderManager.h"

#include "ShaderCompiler.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace render {

	//=================================================================================
	I_RenderNode::I_RenderNode()
		: I_RenderNode(glm::vec3(0.0f), glm::vec3(0.0f))
	{
	}

	//=================================================================================
	inline I_RenderNode::I_RenderNode(const glm::vec3 & pos, const glm::vec3 & rot)
		: m_position(pos)
		, m_rotation(rot)
		, m_modelMatrix(1.0f)
		, m_bShadowCaster(true)
	{
		m_animation = std::make_shared<Animation::C_Animation>(5000.0f);
		m_modelMatrix = glm::translate(glm::mat4(1.0f), pos);
	}

	//=================================================================================
	inline I_RenderNode::~I_RenderNode() {
		DestructorFullCheck();
	}

	//=================================================================================
	void I_RenderNode::RenderChilds(const S_RenderParams& params, const glm::mat4& modelMatrix)
	{
		for (auto & node : *this) {
			node->Render(params, modelMatrix);
		}
	}

	//=================================================================================
	void I_RenderNode::Render(const S_RenderParams& params, const glm::mat4& parentModelMatrix)
	{
		if (!params.m_FrustumSphere.IsColliding(m_bbox.GetSphere())) {
			return;
		}
		const glm::mat4& modelMatrix = parentModelMatrix * m_modelMatrix;
		RenderMyself(params, modelMatrix);
		RenderChilds(params, modelMatrix);
	}

	//=================================================================================
	void I_RenderNode::Clean()
	{
		for (auto & node : *this) {
			node->Clean();
		}
	}

	//=================================================================================
	void I_RenderNode::Update(const double timeSinceLastUpdateMs)
	{
		for (auto & node : *this) {
			node->Update(timeSinceLastUpdateMs);
		}

		//if (m_animation)
		//	m_animation->Update(timeSinceLastUpdateMs);
	}

	//=================================================================================
	void I_RenderNode::PostUpdate()
	{
		for (auto & node : *this) {
			node->PostUpdate();
		}

		if (IsAABBChanged()) {
			m_bbox = AABB();
			for (auto & node : *this) {
				m_bbox.updateWithAABB(GetActualAABB());
			}
		}
	}

	//=================================================================================
	bool I_RenderNode::IsAABBChanged() const
	{
		for (auto & node : *this) {
			if (node->IsAABBChanged())
				return true;
		}
		return false;
	}

	//=================================================================================
	const AABB I_RenderNode::GetActualAABB() const
{
		return m_bbox.getTransformedAABB(GetActualModelMatrix());
	}

	//=================================================================================
	const glm::mat4 I_RenderNode::GetActualModelMatrix() const
	{

		glm::mat4 ret(1.0f);
		ret = glm::translate(ret, m_position);
		// not supported yet
		//if (HasComponent(E_A_Rotation)) {
		//	ret = glm::rotate(ret, m_Components[E_A_Rotation]->update3v(time));
		//}
		//if (HasComponent(E_A_Scale)) {
		//	ret = glm::scale(ret, m_Components[E_A_Scale]->update3v(m_Percentage));
		//}
		return ret * m_animation->GetTRSMatrix();
	}
};
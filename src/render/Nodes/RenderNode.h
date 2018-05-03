/**
 * ==============================================
 * \file 		RenderNode.h
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

#include "Scene.hpp"

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <unordered_set>
#include <vector>

namespace Animation {
	class C_Animation;
}

namespace GLW {
	class C_ShaderProgram;
}

namespace render {
	struct S_RenderParams;

	class I_RenderNode : public std::vector<std::shared_ptr<I_RenderNode>> {
	public:
		I_RenderNode();
		//=================================================================================
		I_RenderNode(const glm::vec3& pos, const glm::vec3& rot);
		virtual ~I_RenderNode();
		virtual void Render(const S_RenderParams& params, const glm::mat4& parentModelMatrix);
		virtual void RenderChilds(const S_RenderParams& params, const glm::mat4& modelMatrix);
		virtual void RenderMyself(const S_RenderParams& params, const glm::mat4& modelMatrix) = 0;
		virtual void Clean();
		virtual void Update(const double timeSinceLastUpdateMs);
		virtual void PostUpdate();
		virtual bool IsAABBChanged() const;

		virtual const AABB GetActualAABB() const;
		virtual const glm::mat4 GetActualModelMatrix() const;

#ifdef _DEBUG
		void SetName(std::string name) { m_name = name; }
		const std::string & GetName() const { return m_name; }
#else
		void SetName(std::string name) {  }
#endif // _DEBUG

		// position and rotation is from parent node
		glm::vec3 m_position;
		glm::vec3 m_rotation;

		AABB m_bbox;

		std::shared_ptr<Animation::C_Animation> m_animation;
		bool IsShadowCaster() const { return m_bShadowCaster; }
		void SetShadowCaster(bool val) { m_bShadowCaster = val; }
	protected:
		glm::mat4 GetAnimationTRS() const;

#ifdef _DEBUG
		std::string m_name;
#endif // _DEBUG
		// todo maybe this should be private?
		glm::mat4 m_modelMatrix;
		bool m_bShadowCaster;
	};
};
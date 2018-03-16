#pragma once

#include <memory>
#include <unordered_set>
#include <vector>
#include <glm/glm.hpp>
#include "Scene.hpp"

#include <GL/glew.h>

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

		// position and rotation is from parent node
		glm::vec3 m_position;
		glm::vec3 m_rotation;

		AABB m_bbox;

		std::shared_ptr<Animation::C_Animation> m_animation;
	protected:
		// todo maybe this should be private?
		glm::mat4 m_modelMatrix;
	};
};
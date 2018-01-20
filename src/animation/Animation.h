#pragma once
#include "animation/AnimationComponent.h"

#include <glm/glm.hpp>

#include <unordered_map>
#include <memory>

// all animations are additive
// just mat4 is multiplicative (and i have no idea how to blend two of them)
namespace Animation {
	class C_Animation {
	public:
					C_Animation(const double lenght);
		void		Start();
		void		Update(const double TimePassed);
		void		AddComponent(std::shared_ptr<I_AnimationComponent> component);
		bool		HasComponent(E_Affected affectedParam) const;
		glm::mat4	GetTRSMatrix();
		glm::vec3	GetParam3f(const E_Affected affectedParam);
		glm::mat4	GetParam4mat(const E_Affected affectedParam);
		int			GetParami(const E_Affected affectedParam);
		double		GetParamf(const E_Affected affectedParam);
	private:
		double m_Lenght;
		double m_TimePassed;
		double m_Percentage;
		std::unordered_map<E_Affected, std::shared_ptr<I_AnimationComponent>> m_Components;
	};
}
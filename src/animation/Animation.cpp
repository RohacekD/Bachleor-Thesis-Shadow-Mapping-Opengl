#include "animation/Animation.h"
#include "animation/AnimationComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace Animation {

	//=================================================================================
	C_Animation::C_Animation(const double lenght)
		: m_Lenght(lenght)
		, m_TimePassed(0.0f)
	{
	}

	//=================================================================================
	void C_Animation::Start()
	{
		for (auto& component : m_Components) {
			component.second->Start();
		}
		m_TimePassed = 0.0f;
		m_Percentage = 0.0f;
	}

	//=================================================================================
	void C_Animation::Update(const double TimePassed)
	{
		m_TimePassed += TimePassed;
		m_Percentage = (m_TimePassed) / m_Lenght;
		//std::cout << m_Percentage << "%" << std::endl;
		if (m_Percentage >= 1.0) {
			Start();// not accurate
		}
	}

	//=================================================================================
	void C_Animation::AddComponent(std::shared_ptr<I_AnimationComponent> component)
	{
		m_Components.insert_or_assign(component->AffectedParameter(), component);
	}

	//=================================================================================
	bool C_Animation::HasComponent(E_Affected affectedParam) const
	{
		return m_Components.find(affectedParam) != m_Components.end();
	}

	//=================================================================================
	glm::mat4 C_Animation::GetTRSMatrix()
{
		glm::mat4 ret(1.0f);
		if (HasComponent(E_A_Translation)) {
			ret = glm::translate(ret, m_Components[E_A_Translation]->update3v(m_Percentage) );
		}
		// not supported yet
		//if (HasComponent(E_A_Rotation)) {
		//	ret = glm::rotate(ret, m_Components[E_A_Rotation]->update3v(time));
		//}
		if (HasComponent(E_A_Scale)) {
			ret = glm::scale(ret, m_Components[E_A_Scale]->update3v(m_Percentage));
		}
		return ret;
	}

	//=================================================================================
	glm::vec3 C_Animation::GetParam3f(const E_Affected affectedParam)
	{
		if (HasComponent(affectedParam)) {
			return m_Components[affectedParam]->update3v(m_Percentage);
		}
		return glm::vec3(0.0f);
	}

	//=================================================================================
	glm::mat4 C_Animation::GetParam4mat(const E_Affected affectedParam)
	{
		if (HasComponent(affectedParam)) {
			return m_Components[affectedParam]->update4mat(m_Percentage);
		}
		return glm::mat4(1.0f);
	}

	//=================================================================================
	int C_Animation::GetParami(const E_Affected affectedParam)
	{
		if (HasComponent(affectedParam)) {
			return m_Components[affectedParam]->updatei(m_Percentage);
		}
		return 0;
	}

	//=================================================================================
	double C_Animation::GetParamf(const E_Affected affectedParam)
	{
		if (HasComponent(affectedParam)) {
			return m_Components[affectedParam]->updatef(m_Percentage);
		}
		return 0.0;
	}
}
#include "animation/AnimCompDecorator.h"

#include <glm/glm.hpp>

namespace Animation {

	//=================================================================================
	C_AnimCompDecorator::C_AnimCompDecorator(std::shared_ptr<I_AnimationComponent> decoree)
		: m_Decoree(decoree)
	{}

	//=================================================================================
	E_Affected C_AnimCompDecorator::AffectedParameter()
	{
		return m_Decoree->AffectedParameter();
	}
	namespace Decorators {

		//=================================================================================
		// C_CompMultiplication
		//=================================================================================
		C_CompMultiplication::C_CompMultiplication(double multiplicator, std::shared_ptr<I_AnimationComponent> decoree)
			: C_AnimCompDecorator(decoree)
			, m_Multiplicator(multiplicator)
		{}

		//=================================================================================
		glm::vec3 C_CompMultiplication::update3v(const double percentage)
		{
			return (float)m_Multiplicator * m_Decoree->update3v(percentage);
		}

		//=================================================================================
		double C_CompMultiplication::updatef(const double percentage)
		{
			return m_Multiplicator * m_Decoree->updatef(percentage);
		}

		//=================================================================================
		int C_CompMultiplication::updatei(const double percentage)
		{
			return static_cast<int>(m_Multiplicator) * m_Decoree->updatei(percentage);
		}

		//=================================================================================
		// C_CompAdd
		//=================================================================================
		C_CompAdd::C_CompAdd(double add, std::shared_ptr<I_AnimationComponent> decoree)
			: C_AnimCompDecorator(decoree)
			, m_Add(add)
		{
		}

		//=================================================================================
		glm::vec3 C_CompAdd::update3v(const double percentage)
		{
			return (float)m_Add + m_Decoree->update3v(percentage);
		}

		//=================================================================================
		double C_CompAdd::updatef(const double percentage)
		{
			return m_Add + m_Decoree->updatef(percentage);
		}

		//=================================================================================
		int C_CompAdd::updatei(const double percentage)
		{
			return static_cast<int>(m_Add) + m_Decoree->updatei(percentage);
		}


	}

}
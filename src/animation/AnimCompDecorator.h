#pragma once

#include "animation/AnimationComponent.h"

#include <memory>

namespace Animation {
	class C_AnimCompDecorator : public I_AnimationComponent {
	public:
		C_AnimCompDecorator(std::shared_ptr<I_AnimationComponent> decoree);
		virtual ~C_AnimCompDecorator() = default;

		virtual E_Affected	AffectedParameter() override;
		protected:
		std::shared_ptr<I_AnimationComponent> m_Decoree;
	};

	namespace Decorators {

		//=================================================================================
		class C_CompMultiplication : public C_AnimCompDecorator {
		public:
			C_CompMultiplication(double multiplicator, std::shared_ptr<I_AnimationComponent> decoree);
			virtual glm::vec3	update3v(const double percentage) override;
			virtual double		updatef(const double percentage) override;
			virtual int			updatei(const double percentage) override;
		protected:
			double m_Multiplicator;
		};

		//=================================================================================
		class C_CompAdd : public C_AnimCompDecorator {
		public:
			C_CompAdd(double add, std::shared_ptr<I_AnimationComponent> decoree);
			virtual glm::vec3	update3v(const double percentage)	override;
			virtual double		updatef(const double percentage)	override;
			virtual int			updatei(const double percentage)	override;
		protected:
			double m_Add;
		};
	}
}
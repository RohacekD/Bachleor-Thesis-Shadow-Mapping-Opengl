#pragma once

#include "animation/AnimationComponent.h"
#include "animation/OneAxisAnimations.h"
#include "animation/AnimCompDecorator.h"

#include <memory>

namespace Animation {

	class C_TranslationAnim : public I_AnimationComponent {
	public:
		virtual ~C_TranslationAnim() {};

		virtual E_Affected AffectedParameter() override
		{
			return E_Affected::E_A_Translation;
		}

		virtual glm::vec3 update3v(const double percentage) override;

		virtual glm::mat4 update4mat(const double percentage) override;

		virtual void Start() override;

		inline void SetXAnimation(std::shared_ptr<I_AnimationComponent> x) { assert(x->AffectedParameter() == E_A_OneAxisTRS); m_XAnim = x; }
		inline void SetYAnimation(std::shared_ptr<I_AnimationComponent> y) { assert(y->AffectedParameter() == E_A_OneAxisTRS); m_YAnim = y; }
		inline void SetZAnimation(std::shared_ptr<I_AnimationComponent> z) { assert(z->AffectedParameter() == E_A_OneAxisTRS); m_ZAnim = z; }

	private:
		std::shared_ptr<I_AnimationComponent> m_XAnim;
		std::shared_ptr<I_AnimationComponent> m_YAnim;
		std::shared_ptr<I_AnimationComponent> m_ZAnim;
	};

	//=================================================================================
	class C_ElipseTranslateAnim : public C_TranslationAnim {
	public:
		C_ElipseTranslateAnim(double x, double y) {
			SetXAnimation(std::make_shared<C_CosAxisComponent>(x));
			SetZAnimation(std::make_shared<C_SinAxisComponent>(y));
			SetYAnimation(std::make_shared<Decorators::C_CompMultiplication>(1.0 / 2.0, std::make_shared<C_SinAxisComponent>(1.0 / 2.0)));
		}
		virtual ~C_ElipseTranslateAnim() {};
	};
}
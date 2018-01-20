#pragma once

#include "animation/AnimationComponent.h"
#include <cmath>
#include <functional>
#include <type_traits>

const double pi = 3.1415926535897;

namespace Animation {
	using namespace std::placeholders;

	// baseMultiplier is here to convert percentage <0,1> to full goniometric circle <0,2)
	template<const int baseMultiplier = 2>
	class I_OneAxisComponentBase : public I_AnimationComponent
	{
	public:
		// use function pointer instead of std::function because otherwise compiler is not able to deduce 
		// overload version of cmath functions
		I_OneAxisComponentBase(double (*func)(double), double multiplier)
			: m_function(func)
			, m_multiplier(multiplier)
		{};

		I_OneAxisComponentBase(std::function<double(double)> func, double multiplier)
			:m_function(func)
		{};
		virtual ~I_OneAxisComponentBase() {};
		// Inherited via I_AnimationComponent
		virtual E_Affected AffectedParameter() override
		{
			return E_Affected::E_A_OneAxisTRS;
		}


		virtual double updatef(const double percentage) override
		{
			return m_function(baseMultiplier * m_multiplier * percentage);
		}
	protected:
		double m_multiplier;
	private:
		std::function<double(double)> m_function;
	};

	//=================================================================================
	class C_SinAxisComponent : public I_OneAxisComponentBase<> {
	public:
		C_SinAxisComponent(double multiplier = 1.0)
			: I_OneAxisComponentBase<>(std::sin, pi * multiplier)
		{}
		virtual ~C_SinAxisComponent() {};
	};

	//=================================================================================
	class C_CosAxisComponent : public I_OneAxisComponentBase<> {
	public:
		C_CosAxisComponent(double multiplier = 1.0)
			: I_OneAxisComponentBase<>(std::cos, pi * multiplier)
		{}
		virtual ~C_CosAxisComponent() {};
	};

	//=================================================================================
	class C_ConstantAxisComponent : public I_OneAxisComponentBase<> {
		
	public:
		C_ConstantAxisComponent(double constant, double multiplier = 1.0)
			: I_OneAxisComponentBase<>(([constant](double a){return constant; }), multiplier)
		{}

		virtual ~C_ConstantAxisComponent() {};
	};
}
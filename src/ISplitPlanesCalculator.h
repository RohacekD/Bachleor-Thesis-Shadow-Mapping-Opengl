/** ==============================================
 * @file 		ISplitPlanesCalculator.h
 * @date 		2018/03/25 23:16
 * @project 	Opengl Shadow Rendering
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include "Camera/ICamera.h"

#include <memory>
#include <vector>
#include <utility>

class C_SplitPlanesStorage;

class I_SplitPlanesCalculator {
public:
	enum class E_MethodType {
		PSSM,
		SDSM
	};
	using T_FrustSplits = std::pair<float, float>;
	using T_FrustVector = std::vector<T_FrustSplits>;

	I_SplitPlanesCalculator(std::shared_ptr<I_Camera> camera);
	virtual ~I_SplitPlanesCalculator();

	void SetLambda(float lambda) { m_ratios->SetLambda(lambda); }

	virtual void BindBuffer(float activate = true);
	virtual const T_FrustVector& GetSplitFrusts() = 0;
	virtual E_MethodType MethodType() const = 0;
protected:
	class C_RatiosCalculator 
		: public I_CameraObserver
	{
	public:
		C_RatiosCalculator(std::shared_ptr<I_Camera> camera, float lambda = 0.5f);
		~C_RatiosCalculator();

		float GetLambda() const { return m_lambda; }
		void SetLambda(float val);

		bool Changed() const { return m_changed; }
		void ChangeProcessed() { m_changed = false; }

		std::vector<float> CalcRations(unsigned int splits);

		virtual void Notify(E_CameraValues value) override;

	protected:
		std::shared_ptr<I_Camera> m_camera;
		float m_lambda;
		bool  m_changed;
	};

	std::shared_ptr<I_Camera> m_camera;
	std::shared_ptr<C_RatiosCalculator> m_ratios; // have to be shared ptr to enable observer pattern. Maybe not a bes idea
	std::shared_ptr<C_SplitPlanesStorage> m_SplitFrust;
};

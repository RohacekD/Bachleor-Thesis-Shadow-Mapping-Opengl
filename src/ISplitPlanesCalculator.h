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

class I_SplitPlanesCalculator {
public:
	using T_FrustSplits = std::pair<float, float>;
	using T_FrustVector = std::vector<T_FrustSplits>;

	I_SplitPlanesCalculator(std::shared_ptr<I_Camera> camera);
	virtual ~I_SplitPlanesCalculator();

	void SetLambda(float lambda) { m_ratios->SetLambda(lambda); }


	virtual const T_FrustVector& GetSplitFrusts() = 0;
protected:
	class C_RatiosCalculator 
		: public I_CameraObserver
	{
	public:
		C_RatiosCalculator(std::shared_ptr<I_Camera> camera, float lambda = 0.5f);
		~C_RatiosCalculator();

		float GetLambda() const { return m_lambda; }
		void SetLambda(float val) { m_lambda = val; m_changed = true; }

		bool Changed() const { return m_changed; }

		std::vector<float> CalcRations(unsigned int splits);

		virtual void Notify(E_CameraValues value) override;

	protected:
		std::shared_ptr<I_Camera> m_camera;
		float m_lambda;
		bool  m_changed;
	};

	std::shared_ptr<I_Camera> m_camera;
	std::shared_ptr<C_RatiosCalculator> m_ratios; // have to be shared ptr to enable observer pattern. Maybe not a bes idea
};

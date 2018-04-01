/** ==============================================
 * @file 		PSSMSplitsCalculator.h
 * @date 		2018/03/26 20:42
 * @project 	Opengl Shadow Rendering
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include "ISplitPlanesCalculator.h"

class I_Camera;

class C_PSSMSplitsCalculator : public I_SplitPlanesCalculator {
public:
	C_PSSMSplitsCalculator(unsigned int frusrums, std::shared_ptr<I_Camera> camera);

	virtual const T_FrustVector& GetSplitFrusts() override;

	void RecalcSplits();

	virtual E_MethodType MethodType() const override;

private:
	T_FrustVector m_frusts;
	unsigned int m_frustums;
};
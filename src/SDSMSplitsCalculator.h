#pragma once

#include "ISplitPlanesCalculator.h"

#include <memory>

class I_Camera;
class C_Histogram;

namespace GLW {
	class C_Texture;
}

class C_SDSMSplitsCalculator : public I_SplitPlanesCalculator {
public:
	C_SDSMSplitsCalculator(unsigned int frusrums, std::shared_ptr<I_Camera> camera);

	void RecalcSplits(std::shared_ptr<GLW::C_Texture> depthMap);

#ifdef _DEBUG
	std::shared_ptr<GLW::C_Texture> GetHistogramTexture() const;
#endif

	virtual const T_FrustVector& GetSplitFrusts() override;

	virtual E_MethodType MethodType() const override;

private:
#ifdef _DEBUG
	std::shared_ptr<GLW::C_Texture> m_HistogramTexture;
#endif
	std::shared_ptr<C_Histogram> m_histogram;

	T_FrustVector m_frusts;
	unsigned int m_frustums;
};
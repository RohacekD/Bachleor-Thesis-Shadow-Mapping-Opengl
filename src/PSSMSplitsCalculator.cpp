#include "PSSMSplitsCalculator.h"

//=================================================================================
C_PSSMSplitsCalculator::C_PSSMSplitsCalculator(unsigned int frusrums, std::shared_ptr<I_Camera> camera)
	: m_frustums(frusrums)
	, I_SplitPlanesCalculator(camera)
{

}

//=================================================================================
const I_SplitPlanesCalculator::T_FrustVector& C_PSSMSplitsCalculator::GetSplitFrusts()
{
	if (m_ratios->Changed()) {
		auto ratios = m_ratios->CalcRations(m_frustums);
		m_frusts = T_FrustVector(m_frustums);
		m_frusts[0].first = m_camera->GetNear();
		for (unsigned int i = 0; i < m_frustums; ++i) {
			m_frusts[i].second = ratios[i];
			if (i != 0) {
				m_frusts[i].first = ratios[i - 1];
			}
		}
		// je to posledni near??????
	}
	return m_frusts;
}

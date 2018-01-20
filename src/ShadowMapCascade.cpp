#include "ShadowMapCascade.h"
#include <iostream>
#include <algorithm>

//=================================================================================
C_ShadowMapCascade::C_ShadowMapCascade(float resolution, int levels, float nearPlane, float farPlane, float m_lambda)
	: m_resolution(resolution)
	, m_levels(levels)
	, m_nearPlane(nearPlane)
	, m_farPlane(farPlane)
	, m_lambda(m_lambda)
{
}

//=================================================================================
void C_ShadowMapCascade::SetLambda(float val) {
	m_lambda = val;
	m_lambda = std::min(std::max(m_lambda, 0.0f), 1.0f);
}

//=================================================================================
void C_ShadowMapCascade::ChangeLambda(float diff)
{
	SetLambda(GetLambda() + diff);
}

//=================================================================================
void C_ShadowMapCascade::PrintSplittingDepths() const
{
	for (float split : m_splitingPlanes) {
		std::cout << split << " ";
	}
	std::cout << std::endl;
}

//=================================================================================
void C_ShadowMapCascade::SetFarPlane(float val) {
	m_farPlane = val; 
	CalcSplitPlanes();
}

//=================================================================================
void C_ShadowMapCascade::SetNearPlane(float val) {
	m_nearPlane = val;
	CalcSplitPlanes();
}

//=================================================================================
void C_ShadowMapCascade::CalcSplitPlanes()
{
	m_splitingPlanes = std::vector<float>();
	m_splitingPlanes.resize(m_levels - 1);
	float logArgument = (GetFarPlane() / GetNearPlane());
	float uniArgument = (GetFarPlane() - GetNearPlane());
	for (unsigned int i = 1; i <= m_levels - 1; ++i) {
		float parameter = static_cast<float>(i) / static_cast<float>(m_levels);
		//std::cout << camera.GetNear() * std::pow(logArgument, parameter) << " - " << camera.GetNear() + uniArgument * parameter<<std::endl;
		m_splitingPlanes[i-1] = (GetLambda() *(GetNearPlane() * std::pow(logArgument, parameter))) + ((1.0 - GetLambda())*(GetNearPlane() + uniArgument * parameter));
		// mapping to [0, 1] interval
		//params.m_planes[i - 1] -= camera.GetNear();
		//params.m_planes[i - 1] /= camera.GetFar() - camera.GetNear();
	}
}

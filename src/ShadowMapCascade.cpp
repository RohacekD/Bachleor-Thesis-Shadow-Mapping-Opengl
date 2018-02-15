#include "ShadowMapCascade.h"

#include "Debug.h"
#include "Frustum.h"
#include "Scene.hpp"
#include "Application.hpp"
#include "LightInfo.h"
#include "CameraManager.h"
#include "Camera/ICamera.h"

#include <iostream>
#include <algorithm>
#include <sstream>

//=================================================================================
C_ShadowMapCascade::C_ShadowMapCascade(std::shared_ptr<C_LightInfo> lightInfo, float resolution, int levels, float m_lambda /*= 0.5f*/) 
	: m_resolution(resolution)
	, m_levels(levels)
	, m_lambda(m_lambda)
	, m_lighInfo(lightInfo)
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
	for (const double split : m_splitingPlanes) {
		std::cout << split << " ";
	}
	std::cout << std::endl;
}

//=================================================================================
void C_ShadowMapCascade::RecalcAll()
{
	CalcSplitPlanes();
	CalcCropMatrices();
}

//=================================================================================
glm::vec4 C_ShadowMapCascade::GetPlanes() const
{
	//todo change to different sizes
	return glm::vec4(m_splitingPlanes[1], m_splitingPlanes[2], m_splitingPlanes[3], m_splitingPlanes[4]);
}

//=================================================================================
void C_ShadowMapCascade::DebugDrawAABBs(const glm::mat4& projectionMatrix) const
{
	int i = 0;
	for (const auto & aabb : m_bboxes) {
		std::stringstream ss;
		ss << "Cascade level: " << i;
		std::string s = ss.str();
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(s.length()), s.c_str());
		C_DebugDraw::Instance().DrawAABB(aabb, projectionMatrix, glm::vec3(1.0f, 0.5f, 1.f));
		glPopDebugGroup();
		++i;
	}
}

//=================================================================================
void C_ShadowMapCascade::CalcSplitPlanes()
{
	auto camera = Application::Instance().GetCamManager()->GetMainCamera();
	m_splitingPlanes = std::vector<double>();
	m_splitingPlanes.resize(m_levels + 1);
	m_splitingPlanes[0] = camera->GetNear();
	float logArgument = (camera->GetFar() / camera->GetNear());
	float uniArgument = (camera->GetFar() - camera->GetNear());
	for (unsigned int i = 1; i <= m_levels - 1; ++i) {
		float parameter = static_cast<float>(i) / static_cast<float>(m_levels);
		m_splitingPlanes[i] = (GetLambda() *(camera->GetNear() * std::pow(logArgument, parameter))) + ((1.0 - GetLambda())*(camera->GetNear() + uniArgument * parameter));
	}
	m_splitingPlanes[m_levels] = camera->GetFar();
}

//=================================================================================
void C_ShadowMapCascade::CalcCropMatrices()
{
	auto camera = Application::Instance().GetCamManager()->GetMainCamera();
	auto projectionMatrix = Application::Instance().GetCamManager()->GetActiveCamera()->getViewProjectionMatrix();
	C_Frustum frust = camera->getFrustum();
	frust.UpdateWithMatrix(m_lighInfo->GetViewMatrix());
	frust.DebugDraw(glm::vec3(1.0f, 1.f, 1.f));
	m_splitInfos = std::vector<S_SplitInfo>();
	m_splitInfos.resize(m_levels);
	for (unsigned int i = 0; i < m_levels; ++i) {
		frust.SetNear(m_splitingPlanes[i]);
		frust.SetFar(m_splitingPlanes[i+1]);
		AABB subFrustBBox = frust.GetAABB();


		C_DebugDraw::Instance().DrawAABB(subFrustBBox, projectionMatrix, glm::vec3(1.0f, 0.5f, 1.f));
		//here was z but IMHO y is correct
		subFrustBBox.minPoint.y = 0.0f;
		// Create the crop matrix
		float scaleX, scaleY, scaleZ;
		float offsetX, offsetY, offsetZ;
		scaleX = 2.0f / (subFrustBBox.maxPoint.x - subFrustBBox.minPoint.x);
		scaleY = 2.0f / (subFrustBBox.maxPoint.y - subFrustBBox.minPoint.y);
		offsetX = -0.5f * (subFrustBBox.maxPoint.x + subFrustBBox.minPoint.x) * scaleX;
		offsetY = -0.5f * (subFrustBBox.maxPoint.y + subFrustBBox.minPoint.y) * scaleY;
		scaleZ = 1.0f / (subFrustBBox.maxPoint.z - subFrustBBox.minPoint.z);
		offsetZ = -subFrustBBox.minPoint.z * scaleZ;

		m_splitInfos[i].m_cropMat = glm::mat4
		   (scaleX,	0.0f,	0.0f,	offsetX,
			0.0f,	scaleY, 0.0f,	offsetY,
			0.0f,	0.0f,	scaleZ, offsetZ,
			0.0f,	0.0f,	0.0f,	1.0f);

		//frust.DebugDraw(glm::vec3(1.0f, 1.f, 1.f));
		//C_DebugDraw::Instance().DrawAABB(subFrustBBox, projectionMatrix, glm::vec3(1.0f, 0.5f, 1.f));
	}
}

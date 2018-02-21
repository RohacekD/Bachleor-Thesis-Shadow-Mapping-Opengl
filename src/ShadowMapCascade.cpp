#include "ShadowMapCascade.h"

#include "Debug.h"
#include "Frustum.h"
#include "Scene.hpp"
#include "Application.hpp"
#include "LightInfo.h"
#include "CameraManager.h"
#include "Camera/ICamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <algorithm>
#include <sstream>

//=================================================================================
C_ShadowMapCascade::C_ShadowMapCascade(std::shared_ptr<C_LightInfo> lightInfo, unsigned int defaultResolution, unsigned int levels, float m_lambda /*= 0.5f*/) 
	: m_resolution(defaultResolution)
	, m_levels(levels)
	, m_lambda(m_lambda)
	, m_lighInfo(lightInfo)
{
}

//=================================================================================
C_ShadowMapCascade::~C_ShadowMapCascade()
{
	DestructorFullCheck();
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
const std::vector<float> C_ShadowMapCascade::GetPlanes() const
{
	//todo change to different sizes
	return std::vector<float>(++(m_splitingPlanes.begin()), m_splitingPlanes.end());
}

//=================================================================================
void C_ShadowMapCascade::DebugDrawAABBs(const glm::mat4& projectionMatrix) const
{
	int i = 0;
	for (const auto & aabb : m_bboxes) {
		RenderDoc::C_DebugScope scope("Cascade level: " + i);
		C_DebugDraw::Instance().DrawAABB(aabb, projectionMatrix, glm::vec3(1.0f, 0.5f, 1.f));
		++i;
	}
}

//=================================================================================
void C_ShadowMapCascade::CalcSplitPlanes()
{
	auto camera = Application::Instance().GetCamManager()->GetMainCamera();
	m_splitingPlanes = std::vector<float>();
	m_splitingPlanes.resize(m_levels + 1);
	m_splitingPlanes[0] = camera->GetNear();
	float logArgument = (camera->GetFar() / camera->GetNear());
	float uniArgument = (camera->GetFar() - camera->GetNear());
	for (unsigned int i = 1; i <= m_levels - 1; ++i) {
		float parameter = static_cast<float>(i) / static_cast<float>(m_levels);
		double result = (GetLambda() *(camera->GetNear() * std::pow(logArgument, parameter))) + ((1.0 - GetLambda())*(camera->GetNear() + uniArgument * parameter));
		m_splitingPlanes[i] = static_cast<float>(result);
	}
	m_splitingPlanes[m_levels] = camera->GetFar();
}

//=================================================================================
void C_ShadowMapCascade::CalcCropMatrices()
{
	using namespace glm;
	const static float nearPlane = 0.0f;
	auto camera = Application::Instance().GetCamManager()->GetMainCamera();
	auto projectionMatrix = Application::Instance().GetCamManager()->GetActiveCamera()->getViewProjectionMatrix();
	C_Frustum frust = camera->getFrustum();
	glm::mat4 lightViewMatrix = m_lighInfo->GetViewMatrix();
	frust.UpdateWithMatrix(lightViewMatrix);
	frust.DebugDraw(glm::vec3(1.0f, 1.f, 1.f));
	m_splitInfos = std::vector<S_SplitInfo>();
	m_splitInfos.resize(m_levels);

	glm::mat4 rotation = glm::mat4_cast(m_lighInfo->GetRotation());
	for (unsigned int i = 0; i < m_levels; ++i) {
		frust.SetNear(m_splitingPlanes[i]);
		frust.SetFar(m_splitingPlanes[i+1]);
		AABB subFrustBBox = frust.GetAABB();
		C_DebugDraw::Instance().DrawAABB(subFrustBBox, projectionMatrix, glm::vec3(1.0f, 0.5f, 1.f));

		float width = subFrustBBox.maxPoint.y - subFrustBBox.minPoint.y;
		float height = subFrustBBox.maxPoint.x - subFrustBBox.minPoint.x;
		glm::mat4 lightProjectionMatrix = glm::ortho(-height / 2.0f,
			height / 2.0f,
			width / 2.0f,
			-width / 2.0f,
			nearPlane,
			nearPlane + subFrustBBox.maxPoint.z - subFrustBBox.minPoint.z);

		vec4 normal = m_lighInfo->GetNormal();

		vec3 center = (subFrustBBox.maxPoint + subFrustBBox.minPoint) / 2.0f;

		vec3 midPointOnBottom = vec3(center.x, center.y, subFrustBBox.maxPoint.z - nearPlane);

		vec4 eye(inverse(lightViewMatrix) * vec4(midPointOnBottom, 1.0f));

		glm::vec4 up(inverse(lightViewMatrix) * vec4(center.x, subFrustBBox.maxPoint.y - nearPlane, center.z - 1.0f, 1.0f));

		up = normalize(up);


		glm::mat4 lightViewMatrix = lookAt(vec3(eye), vec3(eye + normal), vec3(up - eye));

		C_DebugDraw::Instance().DrawPoint(eye, projectionMatrix, glm::vec3(1, 0, 0));
		C_DebugDraw::Instance().DrawPoint(eye + normal, projectionMatrix, glm::vec3(0, 0, 0));
		C_DebugDraw::Instance().DrawAABB(subFrustBBox, projectionMatrix, glm::vec3(0.0f));
		

		m_splitInfos[i].m_lightViewProjectionMatrix = lightProjectionMatrix * lightViewMatrix;
	}
}

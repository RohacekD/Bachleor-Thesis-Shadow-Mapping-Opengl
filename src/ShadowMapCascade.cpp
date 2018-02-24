#include "ShadowMapCascade.h"

#include "Debug.h"
#include "Frustum.h"
#include "Scene.hpp"
#include "Application.hpp"
#include "LightInfo.h"
#include "CameraManager.h"
#include "Camera/ICamera.h"
#include "UniformBuffersManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <algorithm>
#include <sstream>

//=================================================================================
C_ShadowMapCascade::C_ShadowMapCascade(std::shared_ptr<C_LightInfo> lightInfo, std::shared_ptr<I_Camera> camera, unsigned int defaultResolution, unsigned int levels, float m_lambda /*= 0.5f*/)
	: m_resolution(defaultResolution)
	, m_boundCamera(camera)
	, m_levels(levels)
	, m_lambda(m_lambda)
	, m_lighInfo(lightInfo)
{
	//todo
	m_PSSSMUBO = C_UniformBuffersManager::Instance().CreateUniformBuffer<C_PSSMUBO<4>>("PSSM");
	ErrorCheck();
	m_PSSSMUBO->m_splits = levels;
}

//=================================================================================
C_ShadowMapCascade::~C_ShadowMapCascade()
{
	DestructorFullCheck();
}

//=================================================================================
void C_ShadowMapCascade::ActivateUBO(bool activate /*= true*/)
{
	m_PSSSMUBO->Activate(activate);
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
void C_ShadowMapCascade::Update()
{
	const auto func = [](float value, float from1, float to1, float from2, float to2) {
		return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
	};

	RecalcAll();

	m_PSSSMUBO->m_splitPlanes = GetPlanes();
	m_PSSSMUBO->m_cameraView = m_boundCamera->getViewMatrix();
	m_PSSSMUBO->m_cameraProjection = m_boundCamera->getProjectionMatrix();
	for (unsigned int i = 0; i < GetNumLevels(); ++i) {
		const auto& splitInfo = GetSplitInfo(i);
		m_PSSSMUBO->m_lightViewProjections[i] = splitInfo.m_lightViewProjectionMatrix;//splitInfo.m_cropMat * CSMviewProjection
	}
	m_PSSSMUBO->UploadData();
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
	m_splitingPlanes = std::vector<float>();
	m_splitingPlanes.resize(m_levels + 1);
	m_splitingPlanes[0] = m_boundCamera->GetNear();
	float logArgument = (m_boundCamera->GetFar() / m_boundCamera->GetNear());
	float uniArgument = (m_boundCamera->GetFar() - m_boundCamera->GetNear());
	for (unsigned int i = 1; i <= m_levels - 1; ++i) {
		float parameter = static_cast<float>(i) / static_cast<float>(m_levels);
		double result = (GetLambda() *(m_boundCamera->GetNear() * std::pow(logArgument, parameter))) + ((1.0 - GetLambda())*(m_boundCamera->GetNear() + uniArgument * parameter));
		m_splitingPlanes[i] = static_cast<float>(result);
	}
	m_splitingPlanes[m_levels] = m_boundCamera->GetFar();
}

//=================================================================================
void C_ShadowMapCascade::CalcCropMatrices()
{
	using namespace glm;
	const static float nearPlane = 0.0f;
	auto projectionMatrix = Application::Instance().GetCamManager()->GetActiveCamera()->getViewProjectionMatrix();
	C_Frustum frust = m_boundCamera->getFrustum();
	C_Frustum dummyfrust = m_boundCamera->getFrustum();
	glm::mat4 lightViewMatrix = m_lighInfo->GetViewMatrix();
	frust.UpdateWithMatrix(lightViewMatrix);
	frust.DebugDraw(glm::vec3(1.0f, 1.f, 1.f));
	m_splitInfos = std::vector<S_SplitInfo>();
	m_splitInfos.resize(m_levels);

	for (unsigned int i = 0; i < m_levels; ++i) {
		frust.SetNear(m_splitingPlanes[i]);
		frust.SetFar(m_splitingPlanes[i + 1]);
		dummyfrust.SetNear(m_splitingPlanes[i]);
		dummyfrust.SetFar(m_splitingPlanes[i + 1]);
		AABB subFrustBBox = frust.GetAABB();
		dummyfrust.DebugDraw({ 1.0f, 1.0f, 0.0f });
		C_DebugDraw::Instance().DrawAABB(subFrustBBox, projectionMatrix, glm::vec3(1.0f, 0.5f, 1.f));
		C_DebugDraw::Instance().DrawAABB(dummyfrust.GetAABB(), projectionMatrix, glm::vec3(1.0f, 0.5f, 1.f));

		float height = subFrustBBox.maxPoint.y - subFrustBBox.minPoint.y;
		float width = subFrustBBox.maxPoint.x - subFrustBBox.minPoint.x;
		glm::mat4 lightProjectionMatrix = glm::ortho(-width / 2.0f,
			width / 2.0f,
			height / 2.0f,
			-height / 2.0f,
			nearPlane,
			nearPlane + subFrustBBox.maxPoint.z - subFrustBBox.minPoint.z);

		vec4 normal = m_lighInfo->GetNormal();

		vec3 center = (subFrustBBox.maxPoint + subFrustBBox.minPoint) / 2.0f;

		vec3 midPointOnFront = vec3(center.x, center.y, subFrustBBox.maxPoint.z - nearPlane);

		vec4 eye(inverse(lightViewMatrix) * vec4(midPointOnFront, 1.0f));

		vec4 up = vec4(midPointOnFront, 1.0f);
		up.z -= nearPlane;
		up.y += 1.0f;

		up = inverse(lightViewMatrix) * up;

		up = normalize(up);


		glm::mat4 lightViewMatrix = lookAt(vec3(eye), vec3(eye + normal), vec3(up - eye));

		C_DebugDraw::Instance().DrawPoint(eye, projectionMatrix, glm::vec3(1, 0, 0));
		C_DebugDraw::Instance().DrawPoint(glm::normalize(eye + normal), projectionMatrix, glm::vec3(0, 0, 0));
		C_DebugDraw::Instance().DrawPoint(eye + up, projectionMatrix, glm::vec3(1,1,1));
		C_DebugDraw::Instance().DrawPoint(midPointOnFront, projectionMatrix, glm::vec3(1, 0, 0));
		C_DebugDraw::Instance().DrawPoint(glm::normalize(vec4(midPointOnFront, 1.0f) + glm::normalize(lightViewMatrix*normal)), projectionMatrix, glm::vec3(0, 0, 0));
		C_DebugDraw::Instance().DrawAABB(subFrustBBox, projectionMatrix, glm::vec3(0.0f));
		//C_DebugDraw::Instance().DrawAABB(subFrustBBox.getTransformedAABB(), projectionMatrix, glm::vec3(0.0f));
		

		m_splitInfos[i].m_lightViewProjectionMatrix = lightProjectionMatrix * lightViewMatrix;

		subFrustBBox.minPoint.z = 0.0f;
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
		(scaleX, 0.0f, 0.0f, offsetX,
			0.0f, scaleY, 0.0f, offsetY,
			0.0f, 0.0f, scaleZ, offsetZ,
			0.0f, 0.0f, 0.0f, 1.0f);
	}
}

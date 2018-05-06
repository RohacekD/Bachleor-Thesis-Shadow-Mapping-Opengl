#include "ShadowMapCascade.h"

#include "Debug.h"
#include "Frustum.h"
#include "Scene.hpp"
#include "Application.hpp"
#include "LightInfo.h"
#include "CameraManager.h"
#include "Camera/ICamera.h"
#include "UniformBuffersManager.h"

#include "SDSMSplitsCalculator.h"
#include "PSSMSplitsCalculator.h"

#include "Shapes.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <algorithm>
#include <sstream>

//=================================================================================
C_ShadowMapCascade::C_ShadowMapCascade(std::shared_ptr<C_LightInfo> lightInfo, std::shared_ptr<I_Camera> camera, unsigned int defaultResolution, unsigned int levels, std::shared_ptr<I_SplitPlanesCalculator> SplitCalculator, float m_lambda /*= 0.5f*/)
	: m_resolution(defaultResolution)
	, m_boundCamera(camera)
	, m_levels(levels)
	, m_lambda(m_lambda)
	, m_lighInfo(lightInfo)
	, m_SplitCalculator(SplitCalculator)
{
	m_PSSSMUBO = C_UniformBuffersManager::Instance().CreateUniformBuffer<C_PSSMUBO>("PSSM", levels);

	ErrorCheck();
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
	m_SplitCalculator->BindBuffer(activate);
	
}

//=================================================================================
void C_ShadowMapCascade::SetLambda(float val) {
	m_lambda = val;
	m_lambda = std::min(std::max(m_lambda, 0.0f), 1.0f);
	m_SplitCalculator->SetLambda(m_lambda);
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

	m_PSSSMUBO->m_cameraView = m_boundCamera->getViewMatrix();
	m_PSSSMUBO->m_cameraProjection = m_boundCamera->getProjectionMatrix();
	for (unsigned int i = 0; i < GetNumLevels(); ++i) {
		const auto& splitInfo = GetSplitInfo(i);
		m_PSSSMUBO->m_lightViewProjections[i] = splitInfo.m_lightViewProjectionMatrix;//splitInfo.m_cropMat * CSMviewProjection
	}
	m_PSSSMUBO->m_SunDirection = glm::vec3(m_lighInfo->GetNormal());
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
void C_ShadowMapCascade::SetSplittingMethod(I_SplitPlanesCalculator::E_MethodType methodType)
{
	if (methodType != m_SplitCalculator->MethodType()) {
		if (methodType == I_SplitPlanesCalculator::E_MethodType::SDSM) {
			m_SplitCalculator = std::make_shared<C_SDSMSplitsCalculator>(m_levels, GetBoundCamera());
		}
		else {
			m_SplitCalculator = std::make_shared<C_PSSMSplitsCalculator>(m_levels, GetBoundCamera());
		}
	}
}

//=================================================================================
void C_ShadowMapCascade::RecalcAll()
{
	CalcViewProjectionMatrices();
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
Shapes::S_Sphere C_ShadowMapCascade::GetLightFrustumSphere() const
{
	//AABB bboxInLightSpace = GetBBoxInLightSpace(m_boundCamera->getFrustum());
	//Shapes::S_Sphere sphere = bboxInLightSpace.GetSphere();
	//sphere.Transform(glm::inverse(m_lighInfo->GetViewMatrix()));
	return m_boundingSphere;
}

//=================================================================================
AABB C_ShadowMapCascade::GetBBoxInLightSpace(const C_Frustum& frustum) const
{
	return frustum.GetAABB().getTransformedAABB(m_lighInfo->GetViewMatrix());
}

//=================================================================================
void C_ShadowMapCascade::CalcViewProjectionMatrices()
{
	using namespace glm;
	const static float nearPlane = 0.0f;
	auto projectionMatrix = Application::Instance().GetCamManager()->GetActiveCamera()->getViewProjectionMatrix();
	C_Frustum frust = m_boundCamera->getFrustum();
	glm::mat4 lightViewMatrix = m_lighInfo->GetViewMatrix();

	frust.DebugDraw(glm::vec3(1.0f, 1.f, 1.f));

	m_splitInfos = std::vector<S_SplitInfo>();
	m_splitInfos.resize(m_levels);

	auto splits = m_SplitCalculator->GetSplitFrusts();

	// this makes subfrustums bigger, it is virtual distance from frustum to light
	static const float s_lightDistance = 50.0f;

	// get bounding sphere for whole lightspace
	auto wholeLightspaceAABB = frust.GetAABB();
	wholeLightspaceAABB = wholeLightspaceAABB.getTransformedAABB(lightViewMatrix);
	// increase the size of bbox towards sun
	wholeLightspaceAABB.maxPoint.z += s_lightDistance;

	m_boundingSphere = wholeLightspaceAABB.GetSphere();
	m_boundingSphere.Transform(glm::inverse(lightViewMatrix));


	for (unsigned int i = 0; i < m_levels; ++i) {
		frust.SetNear(splits[i].first);
		frust.SetFar(splits[i].second);
		AABB subFrustBBox = frust.GetAABB();
		subFrustBBox = subFrustBBox.getTransformedAABB(lightViewMatrix);
		frust.DebugDraw({ 1.0f, 1.0f, 0.0f });
		// increase the size of bbox towards sun
		subFrustBBox.maxPoint.z += s_lightDistance;
		C_DebugDraw::Instance().DrawAABB(subFrustBBox, projectionMatrix, glm::vec3(1.0f, 0.5f, 1.f), inverse(lightViewMatrix));
		//C_DebugDraw::Instance().DrawAABB(frust.GetAABB(), projectionMatrix, glm::vec3(1.0f, 0.5f, 1.f));

		float height = subFrustBBox.maxPoint.y - subFrustBBox.minPoint.y;
		float width = subFrustBBox.maxPoint.x - subFrustBBox.minPoint.x;
		float lightSpaceDepth = subFrustBBox.maxPoint.z - subFrustBBox.minPoint.z;
		glm::mat4 lightProjectionMatrix = glm::ortho(
			-width / 2.0f,			// left
			width / 2.0f,			// right
			-height / 2.0f,			// bottom
			height / 2.0f,			// top
			nearPlane,
			nearPlane + lightSpaceDepth);

		vec4 normal = m_lighInfo->GetNormal();

		vec3 center = (subFrustBBox.maxPoint + subFrustBBox.minPoint) / 2.0f;

		vec3 midPointOnFront = vec3(center.x, center.y, subFrustBBox.maxPoint.z - nearPlane);
		//eye position have to be in world space
		vec4 eye(inverse(lightViewMatrix) * vec4(midPointOnFront, 1.0f));

		vec4 up = vec4(midPointOnFront, 1.0f);
		up.z -= nearPlane;
		up.y += 1.0f;

		up = inverse(lightViewMatrix) * up;

		up = normalize(up);


		glm::mat4 lightViewMatrixNew = lookAt(vec3(eye), vec3(eye + normal), vec3(up - eye));

		C_DebugDraw::Instance().DrawPoint(eye, projectionMatrix, glm::vec3(1, 0, 0));
		

		m_splitInfos[i].m_lightViewProjectionMatrix = lightProjectionMatrix * lightViewMatrixNew;
		m_splitInfos[i].m_boundingSphere = subFrustBBox.GetSphere();
		C_DebugDraw::Instance().DrawPoint(m_splitInfos[i].m_boundingSphere.m_position, projectionMatrix, glm::vec3(1.0f, 0.5f, 1.f), inverse(lightViewMatrix));
		m_splitInfos[i].m_boundingSphere.Transform(glm::inverse(lightViewMatrix));

		C_DebugDraw::Instance().DrawPoint(m_splitInfos[i].m_boundingSphere.m_position, projectionMatrix, vec3(0.0, 1.0, 0.0));

		//subFrustBBox.minPoint.z = 0.0f;
		//// Create the crop matrix
		//float scaleX, scaleY, scaleZ;
		//float offsetX, offsetY, offsetZ;
		//scaleX = 2.0f / (subFrustBBox.maxPoint.x - subFrustBBox.minPoint.x);
		//scaleY = 2.0f / (subFrustBBox.maxPoint.y - subFrustBBox.minPoint.y);
		//offsetX = -0.5f * (subFrustBBox.maxPoint.x + subFrustBBox.minPoint.x) * scaleX;
		//offsetY = -0.5f * (subFrustBBox.maxPoint.y + subFrustBBox.minPoint.y) * scaleY;
		//scaleZ = 1.0f / (lightSpaceDepth);
		//offsetZ = -subFrustBBox.minPoint.z * scaleZ;
		//
		//m_splitInfos[i].m_cropMat = glm::mat4
		//(scaleX, 0.0f, 0.0f, offsetX,
		//	0.0f, scaleY, 0.0f, offsetY,
		//	0.0f, 0.0f, scaleZ, offsetZ,
		//	0.0f, 0.0f, 0.0f, 1.0f);
	}
}

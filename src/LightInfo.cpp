#include "LightInfo.h"

#include "Application.hpp"
#include "Debug.h"
#include "CameraManager.h"

#include "Camera/ICamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <limits>

//=================================================================================
// C_DirectionalLight
//=================================================================================

const float C_DirectionalLight::s_near = 0.1f;

//=================================================================================
C_DirectionalLight::C_DirectionalLight(std::shared_ptr<I_Camera> camera, const glm::vec3& origin, const glm::vec3& direction, float power, const glm::vec3& color /*= glm::vec3(1.0f, 1.0f, 1.0f)*/)
	: m_origin(origin)
	, m_direciton(direction)
	, m_power(power)
	, m_color(color)
	, m_camera(camera)
{

}

//=================================================================================
glm::mat4 C_DirectionalLight::GetViewProjectionMatrix() const
{
	return GetProjectionMatrix() * GetViewMatrix();
	
}

//=================================================================================
glm::vec3 C_DirectionalLight::GetColor() const
{
	return m_color;
}

//=================================================================================
float C_DirectionalLight::GetPower() const
{
	return m_power;
}

//=================================================================================
glm::mat4 C_DirectionalLight::GetProjectionMatrix() const
{
	AABB camerasAABB = m_camera->GetAABB();
	glm::vec4 normal = glm::vec4(glm::normalize(m_origin + m_direciton), 1.0f);
	glm::quat q(glm::normalize(m_origin + m_direciton), glm::vec3(0, 1, 0));
	glm::mat4 rotation = glm::mat4(1.0f) * glm::mat4_cast(glm::normalize(q));

	AABB transformedAABB = camerasAABB.getTransformedAABB(rotation);


	AABB cameraAABBInLigthSpace = camerasAABB.getTransformedAABB(GetViewMatrix());
	auto viewProjectionMatrix = Application::Instance().GetCamManager()->GetActiveCamera()->getViewProjectionMatrix();
	C_DebugDraw::Instance().DrawAABB(cameraAABBInLigthSpace, glm::mat4(1.0f), viewProjectionMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
	C_DebugDraw::Instance().DrawAABB(camerasAABB, glm::mat4(1.0f), viewProjectionMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
	C_DebugDraw::Instance().DrawAABB(transformedAABB, glm::mat4(1.0f), viewProjectionMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
	C_DebugDraw::Instance().DrawLine(glm::vec4(m_origin, 1.0f), glm::vec4(m_direciton, 1.0f), viewProjectionMatrix, glm::vec3(1.0f, 1.0f, 0.0f));

	return glm::ortho(transformedAABB.minPoint.x, transformedAABB.maxPoint.x, transformedAABB.minPoint.z, transformedAABB.maxPoint.z, s_near, s_near + transformedAABB.maxPoint.y - transformedAABB.minPoint.y);
}

//=================================================================================
glm::mat4 C_DirectionalLight::GetViewMatrix() const
{
	AABB camerasAABB = m_camera->GetAABB();
	glm::vec3 normal = glm::normalize(m_direciton - m_origin);
	auto viewProjectionMatrix = Application::Instance().GetCamManager()->GetActiveCamera()->getViewProjectionMatrix();
	// looking for near and far => looking just for far, because we can say near is smth like 0.1
	glm::vec3 nearest, futher; // initialize to something really, really far from aabb
	float near = std::numeric_limits<float>::max();
	float far = std::numeric_limits<float>::min();

	auto fnc = [&](const glm::vec3& point) {
		float distance = glm::dot(point + normal, normal);
		//C_DebugDraw::Instance().DrawPoint(point, viewProjectionMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
		if (distance > far) {
			far = distance;
			futher = point;
			return;
		}
		if (distance < near) {
			near = distance;
			nearest = point;
		}
	};

	glm::vec3 size = camerasAABB.maxPoint - camerasAABB.minPoint;

	fnc(camerasAABB.maxPoint);
	fnc(camerasAABB.minPoint);
	fnc(camerasAABB.minPoint + glm::vec3(size.x, 0.0f, 0.0f));
	fnc(camerasAABB.minPoint + glm::vec3(size.x, size.y, 0.0f));
	fnc(camerasAABB.minPoint + glm::vec3(size.x, 0.0f, size.z));
	fnc(camerasAABB.minPoint + glm::vec3(0.0f, size.y, 0.0f));
	fnc(camerasAABB.minPoint + glm::vec3(0.0f, 0.0f, size.z));
	fnc(camerasAABB.minPoint + glm::vec3(0.0f, size.y, size.z));

	C_DebugDraw::Instance().DrawPoint(nearest, viewProjectionMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
	C_DebugDraw::Instance().DrawPoint(futher, viewProjectionMatrix, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 eye = nearest + (-normal * s_near);
	C_DebugDraw::Instance().DrawPoint(eye, viewProjectionMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
	C_DebugDraw::Instance().DrawLine(glm::vec4(eye, 1.0f), glm::vec4(eye + normal, 1.0f), viewProjectionMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
	return glm::lookAt(eye, eye + normal, glm::vec3(0.0f, 1.0f, 0.0f));
}

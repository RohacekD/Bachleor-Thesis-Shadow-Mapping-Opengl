#include "LightInfo.h"

#include "Application.hpp"
#include "Debug.h"
#include "CameraManager.h"
#include "Frustum.h"

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
C_LightInfo::~C_LightInfo()
{
	DestructorFullCheck();
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
	//glm::vec4 normal = glm::normalize(glm::vec4((m_direciton - m_origin), 1.0f));
	//glm::quat q(glm::vec3(normal), glm::vec3(0, 1, 0));
	//glm::mat4 rotation = glm::mat4(1.0f) * glm::mat4_cast(glm::normalize(q));

	//AABB transformedAABB = camerasAABB.getTransformedAABB(rotation);
	AABB transformedAABB = camerasAABB.getTransformedAABB(GetViewMatrix());


	//AABB cameraAABBInLigthSpace = camerasAABB.getTransformedAABB(GetViewMatrix());
	auto viewProjectionMatrix = Application::Instance().GetCamManager()->GetActiveCamera()->getViewProjectionMatrix();
	//C_DebugDraw::Instance().DrawAABB(cameraAABBInLigthSpace, glm::mat4(1.0f), viewProjectionMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
	//C_DebugDraw::Instance().DrawAABB(camerasAABB, glm::mat4(1.0f), viewProjectionMatrix, glm::vec3(0.0f, 1.0f, 0.0f));//aabb kamery
	//C_DebugDraw::Instance().DrawAABB(camerasAABB, viewProjectionMatrix, glm::vec3(1.0f, 0.0f, 0.0f), rotation); // aabb kamery ve light view space 
	//C_DebugDraw::Instance().DrawAABB(transformedAABB, viewProjectionMatrix, glm::vec3(0.0f, 0.0f, 1.0f));//aabb n world space
	//C_DebugDraw::Instance().DrawAABB(transformedAABB, viewProjectionMatrix, glm::vec3(1.0f, 0.0f, 0.0f), glm::inverse(GetViewMatrix()));//aabb n light space


	//C_DebugDraw::Instance().DrawAABB(transformedAABB1, viewProjectionMatrix, glm::vec3(0.0f, 0.0f, 1.0f));//aabb n world space
	//C_DebugDraw::Instance().DrawAABB(transformedAABB1, viewProjectionMatrix, glm::vec3(1.0f, 1.0f, 0.0f), glm::inverse(GetViewMatrix()));//aabb n light space

	C_DebugDraw::Instance().DrawLine(glm::vec4(m_origin, 1.0f), glm::vec4(m_direciton, 1.0f), viewProjectionMatrix, glm::vec3(1.0f, 1.0f, 0.0f));

	float width = transformedAABB.maxPoint.z - transformedAABB.minPoint.z;
	float height = transformedAABB.maxPoint.x - transformedAABB.minPoint.x;
	return glm::ortho(-height / 2.0f,
						height / 2.0f,
						width /2.0f,
					   -width / 2.0f,
						s_near, 
						s_near + transformedAABB.maxPoint.y - transformedAABB.minPoint.y);
}

//=================================================================================
glm::mat4 C_DirectionalLight::GetViewMatrix() const
{
	using namespace glm;
	AABB camerasAABB = m_camera->GetAABB();
	vec4 normal = normalize(vec4((m_direciton - m_origin), 1.0f));
	quat q(vec3(normal), vec3(0, 0, 1));
	mat4 rotation = mat4(1.0f) * mat4_cast(normalize(q));

	AABB transformedAABB = camerasAABB.getTransformedAABB(rotation);

	vec3 center = (transformedAABB.maxPoint + transformedAABB.minPoint) / 2.0f;
	
	vec3 midPointOnBottom = vec3(center.x, center.y, transformedAABB.minPoint.z - s_near);

	vec4 eye(inverse(rotation) * vec4(midPointOnBottom, 1.0f));

	//vec4 foreward(inverse(rotation) * vec4(center.x, transformedAABB.minPoint.y - s_near + 1.0f, center.z, 1.0f));

	vec4 up(inverse(rotation) * vec4(center.x, transformedAABB.minPoint.y - s_near, center.z - 1.0f, 1.0f));

	up = normalize(up);

	auto viewProjectionMatrix = Application::Instance().GetCamManager()->GetActiveCamera()->getViewProjectionMatrix();

	//C_DebugDraw::Instance().DrawAABB(transformedAABB, viewProjectionMatrix, vec3(0.0f, 0.0f, 1.0f), inverse(rotation));//aabb n world
	//C_DebugDraw::Instance().DrawPoint(eye, viewProjectionMatrix, vec3(1.0f, 1.0f, 1.0f));
	//
	//C_DebugDraw::Instance().DrawAxis(eye, up, foreward, viewProjectionMatrix);


	//vec3 color = vec3(1.0f, 0.0f, 0.0f);
	//C_DebugDraw::Instance().DrawPoint(eye, viewProjectionMatrix, color);
	//C_DebugDraw::Instance().DrawPoint(vec3(eye + normal), viewProjectionMatrix, vec3(1.0f, 1.0f, 1.0f));
	//C_DebugDraw::Instance().DrawLine(vec3(eye), vec3(eye + normal), viewProjectionMatrix, vec3(1.0f, 1.0f, 1.0f));
	//C_DebugDraw::Instance().DrawPoint(eye + up, viewProjectionMatrix, vec3(0.0f, 1.0f, 0.0f));
	//C_DebugDraw::Instance().DrawAABB(transformedAABB, viewProjectionMatrix, glm::vec3(0, 0, 0));
	//C_DebugDraw::Instance().DrawAABB(camerasAABB, viewProjectionMatrix, glm::vec3(0, 0, 0));

	return lookAt(vec3(eye), vec3(eye + normal), vec3(up - eye));
}

#include "LightInfo.h"

#include "animation/Animation.h"
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
C_DirectionalLight::C_DirectionalLight(std::shared_ptr<I_Camera> camera, const glm::vec3& origin, const glm::vec3& focusPoint, float power, const glm::vec3& color /*= glm::vec3(1.0f, 1.0f, 1.0f)*/) : m_originAnimation(nullptr)
	, m_directionAnimation(nullptr)
	, m_origin(origin)
	, m_focusPoint(focusPoint)
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
glm::vec3 C_DirectionalLight::GetPosition() const
{
	if(m_originAnimation)
		return glm::vec3(m_originAnimation->GetParam3f(Animation::E_A_Translation) + m_origin);
	return m_origin;
}

//=================================================================================
glm::vec3 C_DirectionalLight::GetFocusPoint() const
{
	if (m_directionAnimation)
		return glm::vec3(m_directionAnimation->GetTRSMatrix() * glm::vec4(m_focusPoint, 1.0f));
	return m_focusPoint;
}

//=================================================================================
glm::mat4 C_DirectionalLight::GetProjectionMatrix() const
{
	AABB camerasAABB = m_camera->GetAABB();
	
	AABB transformedAABB = camerasAABB.getTransformedAABB(GetViewMatrix());

	float width = transformedAABB.maxPoint.z - transformedAABB.minPoint.z;
	float height = transformedAABB.maxPoint.x - transformedAABB.minPoint.x;
	return glm::ortho(-height / 2.0f,
						height / 2.0f,
						width /2.0f,
					   -width / 2.0f,
						s_near, 
						s_near + transformedAABB.maxPoint.z - transformedAABB.minPoint.z);
}

//=================================================================================
glm::mat4 C_DirectionalLight::GetViewMatrix() const
{
	using namespace glm;
	AABB camerasAABB = m_camera->GetAABB();
	vec4 normal = GetNormal();
	quat q = GetRotation();
	mat4 rotation = mat4(1.0f) * mat4_cast(normalize(q));

	AABB transformedAABB = camerasAABB.getTransformedAABB(rotation);

	vec3 center = (transformedAABB.maxPoint + transformedAABB.minPoint) / 2.0f;
	
	vec3 midPointOnBottom = vec3(center.x, center.y, transformedAABB.maxPoint.z - s_near);

	vec4 eye(inverse(rotation) * vec4(midPointOnBottom, 1.0f));

	vec4 up(inverse(rotation) * vec4(center.x, transformedAABB.maxPoint.y - s_near, center.z - 1.0f, 1.0f));

	up = normalize(up);

	return lookAt(vec3(eye), vec3(eye + normal), vec3(up - eye));
}

//=================================================================================
glm::quat C_DirectionalLight::GetRotation() const
{
	using namespace glm;
	vec4 normal = GetNormal();
	quat q(vec3(normal), vec3(0, 0, 1));
	return q;
}

//=================================================================================
glm::vec4 C_DirectionalLight::GetNormal() const
{
	using namespace glm;
	return normalize(vec4((GetFocusPoint() - GetPosition()), 1.0f));
}

//=================================================================================
void C_DirectionalLight::SetOriginAnimation(std::shared_ptr<Animation::C_Animation> anim)
{
	m_originAnimation = anim;
}

//=================================================================================
void C_DirectionalLight::SetDirectionAnimation(std::shared_ptr<Animation::C_Animation> anim)
{
	m_directionAnimation = anim;
}

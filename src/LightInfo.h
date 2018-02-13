#pragma once

#include <glm/glm.hpp>

#include <memory>

class I_Camera;

class C_Frustum;

// still have no idea how to represent power

class C_LightInfo {
public:
	C_LightInfo() = default;
	virtual ~C_LightInfo() = default;

	virtual glm::mat4 GetProjectionMatrix() const = 0;
	virtual glm::mat4 GetViewProjectionMatrix() const = 0;
	virtual glm::mat4 GetViewMatrix() const = 0;


	virtual glm::vec3 GetColor() const = 0;
	virtual float	  GetPower() const = 0;
};

//=================================================================================
// C_DirectionalLight - camera relevant
// Projection here have no meaning
//=================================================================================
class C_DirectionalLight : public C_LightInfo {
public:
	C_DirectionalLight(std::shared_ptr<I_Camera> camera, const glm::vec3& origin, const glm::vec3& direction, float power, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
	virtual glm::mat4	GetViewProjectionMatrix() const override;
	virtual glm::vec3	GetColor() const override;
			void		SetColor(const glm::vec3& color) { m_color = color; }
	virtual float		GetPower() const override;
			void		SetPower(float power) { m_power = power; }

			glm::vec3	GetPosition() const { return m_origin; }
			void		SetPosition(const glm::vec3& position) { m_origin = position; }

	virtual glm::mat4	GetProjectionMatrix() const override;

	virtual glm::mat4	GetViewMatrix() const override;

private:
	float		m_power;
	glm::vec3	m_origin;
	glm::vec3	m_direciton;
	glm::vec3	m_color;
	std::shared_ptr<I_Camera>	m_camera;

	const static float s_near;
};

//=================================================================================
/** ==============================================
 * @file 		LightInfo.h
 * @date 		2018/03/17 19:54
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include "animation/TranslationAnimations.h"

#include <glm/glm.hpp>

#include <memory>

class I_Camera;

class C_Frustum;

// still have no idea how to represent power

class C_LightInfo {
public:
	C_LightInfo() = default;
	virtual ~C_LightInfo();

	virtual glm::mat4 GetProjectionMatrix() const = 0;
	virtual glm::mat4 GetViewProjectionMatrix() const = 0;
	virtual glm::mat4 GetViewMatrix() const = 0;

	//this is here just for now, because I am going to skip idea about perspective light
	virtual glm::quat GetRotation() const = 0;
	virtual glm::vec4 GetNormal() const = 0;


	virtual glm::vec3 GetColor() const = 0;
	virtual float	  GetPower() const = 0;
};


namespace Animation {
	class C_Animation;
}

//=================================================================================
// C_DirectionalLight - camera relevant
// Projection here have no meaning
//=================================================================================
class C_DirectionalLight : public C_LightInfo {
public:
	C_DirectionalLight(std::shared_ptr<I_Camera> camera, const glm::vec3& origin, const glm::vec3& focusPoint, float power, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
	virtual glm::mat4	GetViewProjectionMatrix() const override;
	virtual glm::vec3	GetColor() const override;
			void		SetColor(const glm::vec3& color) { m_color = color; }
	virtual float		GetPower() const override;
			void		SetPower(float power) { m_power = power; }

			glm::vec3	GetPosition() const;
			void		SetPosition(const glm::vec3& position) { m_origin = position; }
			glm::vec3	GetFocusPoint() const;
			void		SetFocusPoint(const glm::vec3& direction) { m_focusPoint = direction; }

	virtual glm::mat4	GetProjectionMatrix() const override;
	virtual glm::mat4	GetViewMatrix() const override;
	virtual glm::quat	GetRotation() const override;
	virtual glm::vec4	GetNormal() const override;

	void    SetOriginAnimation(std::shared_ptr<Animation::C_Animation> anim);
	void    SetDirectionAnimation(std::shared_ptr<Animation::C_Animation> anim);

private:
	std::shared_ptr<Animation::C_Animation> m_originAnimation;
	std::shared_ptr<Animation::C_Animation> m_directionAnimation;
	float						m_power;
	glm::vec3					m_origin;
	glm::vec3					m_focusPoint;
	glm::vec3					m_color;
	std::shared_ptr<I_Camera>	m_camera;

	const static float s_near;
};

//=================================================================================
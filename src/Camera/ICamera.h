/**
 * ==============================================
 * \file 		ICamera.h
 * \date 		2018/03/17 19:08
 * \project 	Computer Graphics Project
 * \faculty 	Faculty of Information Technology 
 * \university 	Brno University of Technology
 *
 * \author Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
 */

#pragma once

#include <glm/glm.hpp>
#include "Scene.hpp"
#include "Frustum.h"
#include <SDL/SDL_events.h>

class I_Camera {
public:
	I_Camera(){}
	virtual ~I_Camera() {};

	// return true if object handles input
	virtual bool Input(SDL_Event) = 0;

	virtual void adjustOrientation(float hRad, float vRad) = 0;

	virtual void update() = 0;

	virtual glm::mat4 getViewProjectionMatrix() const = 0;
	virtual glm::mat4 getViewMatrix() const = 0;
	virtual glm::mat4 getProjectionMatrix() const = 0;
	virtual glm::vec3 getPosition() const = 0;
	virtual glm::vec3 getDirection() const = 0;

	virtual C_Frustum getFrustum() const = 0;

	// In world space!!
	virtual AABB	  GetAABB() const = 0;

	virtual void	  debugDraw() const = 0;

	virtual glm::quat getRotation() const = 0;

	virtual float GetFar() const = 0;
	virtual void  SetFar(float far) = 0;

	virtual float GetNear() const = 0;
	virtual void  SetNear(float near) = 0;
	virtual float GetFov() const = 0;
	virtual void  SetFov(float fov) = 0;
	virtual float GetAspectRatio() const = 0;
};
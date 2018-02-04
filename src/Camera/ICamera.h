#pragma once

#include <glm/glm.hpp>
#include "Scene.hpp"
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

	// In world space!!
	virtual AABB	  GetAABB() const = 0;

	virtual void	  debugDraw() const = 0;

	virtual glm::quat getRotation() const = 0;

	virtual float GetFar() const = 0;
	virtual float GetNear() const = 0;
	virtual float GetFov() const = 0;
	virtual float GetAspectRatio() const = 0;
};
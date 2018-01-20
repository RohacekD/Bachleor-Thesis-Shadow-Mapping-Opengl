#pragma once

#include <glm/glm.hpp>

class I_Camera {
public:
	I_Camera(){}
	virtual ~I_Camera() {};

	virtual glm::mat4 getViewProjectionMatrix() const = 0;
	virtual glm::mat4 getViewMatrix() const = 0;
	virtual glm::mat4 getProjectionMatrix() const = 0;
	virtual glm::vec3 getPosition() const = 0;

	virtual float GetFar() const = 0;
	virtual float GetNear() const = 0;
};
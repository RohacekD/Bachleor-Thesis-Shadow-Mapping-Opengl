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

#include <vector>
#include <memory>

class I_CameraObserver;

enum class E_CameraValues {
	Near,
	Far,
	Fov,
};

class I_Camera {
public:

	virtual ~I_Camera() = default;

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


	void Subscribe(std::shared_ptr<I_CameraObserver> observer);
	void Unsubscribe(std::shared_ptr<I_CameraObserver> observer);
	void Notify(E_CameraValues value);
private:
	std::vector<std::weak_ptr<I_CameraObserver>> m_observers;
};

class I_CameraObserver {
public:
	~I_CameraObserver() = default;
	virtual void Notify(E_CameraValues value) = 0;
};
#pragma once

#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include <GL/glew.h>

#include <memory>

struct Scene;
class I_Camera;

class I_Renderer
{
public:
	I_Renderer() {};
	virtual ~I_Renderer() {}

	virtual bool init(std::shared_ptr<Scene> scene, unsigned int screenWidth, unsigned int screenHeight) = 0;
	virtual void onUpdate(float timeSinceLastUpdateMs) = 0;
	virtual void onWindowRedraw(const I_Camera& camera, const glm::vec3& cameraPosition) = 0;
	virtual void onKeyPressed(SDL_Keycode code) = 0;
	virtual void clearStudentData() = 0;
};

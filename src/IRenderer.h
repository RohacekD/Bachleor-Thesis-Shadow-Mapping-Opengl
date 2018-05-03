/** ==============================================
 * @file 		IRenderer.h
 * @date 		2018/03/17 19:53
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include <GL/glew.h>

#include <string>
#include <memory>

struct Scene;
class I_Camera;

/** ==============================================
 * @class I_Renderer
 *
 * @brief Interface for renderer.
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2018/03/17
 ** ==============================================*/
class I_Renderer
{
public:
	I_Renderer() {};
	virtual ~I_Renderer() {}

	virtual bool init(const std::string& scene, unsigned int screenWidth, unsigned int screenHeight, bool useSDSM = false) = 0;
	virtual void onUpdate(float timeSinceLastUpdateMs) = 0;
	virtual void onWindowRedraw(const I_Camera& camera, const glm::vec3& cameraPosition) = 0;
	virtual void onKeyPressed(SDL_Keycode code) = 0;
	virtual void clearStudentData() = 0;
};

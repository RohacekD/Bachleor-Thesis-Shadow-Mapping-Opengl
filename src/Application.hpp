#pragma once

/*
 * File:   Application.hpp
 * Author: Jozef Kobrtek
 * Date:   24th August 2016
 */

//STL includes
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>

//OpenGL stuff
#include <SDL/SDL.h>
#undef main
#include <GL/glew.h>

//Project includes
#include "HighResolutionTimer.hpp"
#include "SceneLoader.hpp"
#include "Camera/OrbitalCamera.hpp"
#include "Camera/FreelookCamera.hpp"
#include "ShaderCompiler.hpp"
#include "CameraPath.h"
#include "StudentRenderer.hpp"
#include "CameraManager.h"

//Project defines
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

//If commented out, freelook camera is used instead
//#define USE_ORBITAL_CAMERA

class Application
{
	//Camera
#ifdef USE_ORBITAL_CAMERA
	using T_Camera = OrbitalCamera;
#else
	using T_Camera = FreelookCamera;
#endif
public:
	//Singleton stuff changed by DR
	Application(Application const&) = delete;
	void operator=(Application const&) = delete;
	static Application& Instance();

    //Main application function
    bool Run(int argc, char* argv[]);

    //Initializes window and OpenGL
    //Calls initGL function
    bool Init();

	void setupCamera(std::shared_ptr<OrbitalCamera>& orbitalCamera);
	void setupCamera(std::shared_ptr<FreelookCamera>& freeCamera);

    //Clears all resources used by the application
    //Also calls clearGL
    void Clear();
    
    //Initializes SDL, GLEW
    bool initWindow();
    bool initGlew();

    //Destroys window
    void clearSDLWindow();

	inline void AddDrawCall() { _renderer.AddDrawCall(); }

	std::shared_ptr<C_CameraManager> GetCamManager() const { return m_cameraManager; }

	const std::string& GetShaderForMeshes() const { return m_ShaderForMeshes; }
	void SetShaderForMeshes(std::string val) { m_ShaderForMeshes = val; }

	const std::string& GetShaderForMeshesShadows() const { return m_ShaderForMeshesShadows; }
	bool	IsLayeredOn() const { return m_bLayeredRender; }
	void	SetLayered(bool enable = true);

	double GetCameraPathProgress() const { return m_CameraPathProgress; }
	void SetCameraPathProgress(double val) { m_CameraPathProgress = val; }
private:
	Application();

	
	/** ==============================================
	 * @method:    ProcessControls
	 * @fullName:  Application::ProcessControls
	 * @return:    bool true if application should continue execution
	 * @param: 	   std::shared_ptr<I_Camera> camera
	 * @brief	   
	 ** ==============================================*/
	bool ProcessControls(std::shared_ptr<I_Camera> camera);
	bool PorcessControlsCamPath(std::shared_ptr<I_Camera> camera);
	bool ApplyCameraPath(std::shared_ptr<I_Camera> camera);
	bool CameraPathFinished();

	struct {
		bool m_active;
		bool m_controlScene;
		bool m_controlMainCam;
		bool m_useMainCam;
	} m_controlPanel;

#ifndef SPEEDPROFILE
	void ShowGUI();
#endif

	void _splitPathToFilenameAndDirectory(const std::string& path, std::string& directoryPath, std::string& fileName);

    //SDL window and OpenGL context
    SDL_Window*          _window;
    SDL_GLContext        _glContext;

	//Timer
	HighResolutionTimer  _timer;
	HighResolutionTimer  m_CameraPathTimer;

	std::shared_ptr<C_CameraManager>		m_cameraManager;

	std::shared_ptr<T_Camera> m_camera;
    //Scene data
    std::shared_ptr<Scene> _scene;

	//==============================================
	// data for camera path
	//==============================================
	std::shared_ptr<CameraPath> m_camPath;
	double						m_CameraPathProgress;
	bool						m_bPathFinished : 1;

	bool						m_bLayeredRender : 1;

	// stream where to output statistics
	std::ofstream				m_statisticsFile;

	// shader for final rendering of mashes
	std::string					m_ShaderForMeshes;
	std::string					m_ShaderForMeshesShadows;


	//Student renderer
	StudentRenderer		 _renderer;  
};
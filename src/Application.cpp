#include "Application.hpp"

#ifndef SPEEDPROFILE
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl_gl3.h>
#endif

#include "Debug.h"
#include "DebugCallbacks.h"
#include "CameraPathLoader.h"
#include "ShaderManager.h"



//=================================================================================
Application::Application()
	: m_camPath(nullptr)
	, m_bPathFinished(false)
	, _window(nullptr)
{}

//=================================================================================
bool Application::Init()
{
    if (!initWindow())
        return false;

    if (!initGlew())
        return false;

#ifndef SPEEDPROFILE

	glDebugMessageCallback(debugFunc, nullptr);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplSdlGL3_Init(_window);

	ImGui::StyleColorsDark();
#endif

	m_cameraManager = std::make_shared<C_CameraManager>();
	_scene = std::make_shared<Scene>();

	m_controlPanel.m_active = true;
	m_controlPanel.m_controlMainCam = false;
#ifndef SPEEDPROFILE
	m_controlPanel.m_controlScene = true;
#else
	m_controlPanel.m_controlScene = false;
#endif
	m_controlPanel.m_useMainCam = false;

    return true;
}

//=================================================================================
bool Application::initWindow()
{
    // Initialize SDL's Video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL Failed to initialize!\n";
        return false;
    }

    _window = SDL_CreateWindow("OpenGL shadow Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if(_window == nullptr)
    {
        std::cerr << "Failed to create SDL window!\n";
        return false;
    }

    //Set OpenGL version to 4.4, core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    //Double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //Create OpenGL context
    _glContext = SDL_GL_CreateContext(_window);

    //VSync
    SDL_GL_SetSwapInterval(1);

    return true;
}

//=================================================================================
bool Application::initGlew()
{
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW failed to initialize!\n";
        return false;
    }

    return true;
}

//=================================================================================
void Application::clearSDLWindow()
{
	SDL_GL_DeleteContext(_glContext);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

//=================================================================================
void Application::Clear()
{
    _renderer.clearStudentData();
     
	clearSDLWindow();
}

//=================================================================================
void Application::setupCamera(std::shared_ptr<OrbitalCamera>& orbitalCamera)
{
	//Setup camera
	//Make the scene fit the camera's view frustum
	//Camera is static throughout the app, it will be the model that will rotate

	//Create hypothetical bounding sphere around scene's AABB, d - its radius
	//We will create camera's view frustum around this sphere
	float r = glm::length(_scene->bbox.maxPoint - _scene->bbox.minPoint) / 2;
	float fovyDeg = 90;
	float fovyRad = glm::radians(fovyDeg);
	float aspectRatio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
	float nearZ = 0.1f;

	float fovXrad = 2 * glm::asin(aspectRatio * glm::sin(fovyRad / 2.0f));

	//The sphere can touch the frustum either on left-right sides, or top-bottom
	//We choose, which distance is further away
	float d = r / glm::max(sin(fovyRad / 2), sin(fovXrad / 2));

	orbitalCamera->setupCameraProjection(0.1f, 2 * (d + r), aspectRatio, fovyDeg);
	orbitalCamera->setupCameraView(d, _scene->bbox.minPoint + 0.5f*(_scene->bbox.maxPoint - _scene->bbox.minPoint), 0, 0);
	orbitalCamera->update();
}

//=================================================================================
void Application::setupCamera(std::shared_ptr<FreelookCamera>& freeCamera)
{
	//float r = glm::length(_scene->bbox.maxPoint - _scene->bbox.minPoint) / 2;
	float fovyDeg = 90;
	float fovyRad = glm::radians(fovyDeg);
	float aspectRatio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
	//float nearZ = 0.1f;

	//float fovXrad = 2 * glm::asin(aspectRatio * glm::sin(fovyRad / 2.0f));
	//float d = r / glm::max(sin(fovyRad / 2), sin(fovXrad / 2));

	freeCamera->setupCameraProjection(0.1f, 100.0f, aspectRatio, fovyDeg);
	freeCamera->positionCamera(glm::vec3(0, 0, -1), glm::vec3(0));
}

//=================================================================================
#ifndef SPEEDPROFILE
void Application::ShowGUI()
{
	// Create a window called "My First Tool", with a menu bar.
	ImGui::Begin("Controls", &m_controlPanel.m_active);
	ImGui::Checkbox("Disable Controls", &m_controlPanel.m_controlScene);
	ImGui::Checkbox("Use main camera", &m_controlPanel.m_useMainCam);
	ImGui::Checkbox("Control main camera", &m_controlPanel.m_controlMainCam);
	ImGui::End();

}
#endif

//=================================================================================
void Application::_splitPathToFilenameAndDirectory(const std::string& path, std::string& directoryPath, std::string& fileName)
{
	std::size_t found = path.find_last_of("/\\");

	directoryPath = path.substr(0, found);
	fileName = path.substr(found + 1);
}

//=================================================================================
Application& Application::Instance()
{
	static Application    instance; // Guaranteed to be destroyed.
										// Instantiated on first use.
	return instance;
}

//=================================================================================
bool Application::Run(int argc, char* argv[])
{
#ifdef SPEEDPROFILE
	//in speed profile we need to know rendering method
	if (argc < 3) {
		return false;
	}
#else
	if (argc < 2) {
		return false;
	}
#endif

	bool useCamPath = argc == 4;

	bool sdsm = false;
	if (argc > 2) {
		if (!strcmp("s", argv[2])) {
			sdsm = true;
		}
		else if (!strcmp("p", argv[2])) {
			sdsm = false;
		}
	}


    if(!Init())
        return false;

	GetCamManager();

	// we need main camera no matter what
	m_camera = std::make_shared<T_Camera>();
	GetCamManager()->SetMainCamera(m_camera);
	setupCamera(m_camera);

	m_camera->SetFov((60));
	m_camera->SetFar(m_camera->GetFar() / 3);
	m_camera->update();


	//Prepare rendering data
	if (!_renderer.init(argv[1], SCREEN_WIDTH, SCREEN_HEIGHT, sdsm))
		return false;

	if (!useCamPath) {
		auto debugCam = std::make_shared<T_Camera>();
		GetCamManager()->SetDebugCamera(debugCam);
		setupCamera(debugCam);
		debugCam->update();
	}
	else {
		C_CameraPathLoader camPthLoader;
		std::string camPathFile(argv[3]);
		camPathFile.erase(camPathFile.end() - 4, camPathFile.end());
		camPathFile.append("-");
		camPathFile.append(argv[2]);
		camPathFile.append(".csv");
		m_statisticsFile.open(camPathFile);
		_renderer.SetStatisticsOutput(m_statisticsFile);
		_renderer.WriteStatisticsHeader();

		m_camPath = std::make_shared<CameraPath>(camPthLoader.LoadPath(argv[3]));
		auto zeroKey = m_camPath->getKeypoint(0.0f);
		m_camera->positionCamera(zeroKey.position, zeroKey.position + zeroKey.viewVector, zeroKey.upVector);
		m_camera->update();
		m_CameraPathTimer.reset();
	}
	GetCamManager()->UseDebugCam(!useCamPath);


    _timer.reset();
    bool quit = false;

	while (!quit)
	{

		auto renderCamera = GetCamManager()->GetActiveCamera();

		if (useCamPath) {
			quit = PorcessControlsCamPath(GetCamManager()->GetMainCamera()) != true;
		}
		else {

			auto controledCamera = renderCamera;
			if (m_controlPanel.m_controlMainCam) {
				controledCamera = GetCamManager()->GetMainCamera();
			}
			quit = ProcessControls(controledCamera) != true;
		}

#ifndef SPEEDPROFILE
		ImGui_ImplSdlGL3_NewFrame(_window);
#endif
		// strip down all we can when using cam path
		if (useCamPath) {

		}
		else {
			GetCamManager()->UseDebugCam(!m_controlPanel.m_useMainCam);
			ErrorCheck();

		}

		renderCamera->update();
		m_camera->update();
        _renderer.onUpdate(float(_timer.getElapsedTimeFromLastQueryMilliseconds()));

		_renderer.onWindowRedraw(*(renderCamera.get()), renderCamera->getPosition());
#ifndef SPEEDPROFILE
		ShowGUI();
		{
			C_ShaderManager::Instance().DeactivateShader();
			RenderDoc::C_DebugScope a("imGUI-render");
			ImGui::Render();
			ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
			ErrorCheck();
		}
#endif
        SDL_GL_SwapWindow(_window);
    }

#ifndef SPEEDPROFILE
	ImGui_ImplSdlGL3_Shutdown();
#endif
    Clear();

    return true;
}

//=================================================================================
bool Application::ProcessControls(std::shared_ptr<I_Camera> camera)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
#ifndef SPEEDPROFILE
		ImGui_ImplSdlGL3_ProcessEvent(&e);
#endif
		if (m_controlPanel.m_controlScene) continue;
		camera->Input(e);
		switch (e.type)
		{
		case SDL_QUIT:
			return false;
			break;

		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE)
				return false;
			else if (e.key.keysym.sym == SDLK_v) {
				m_controlPanel.m_controlMainCam = true;
			}
			else if (e.key.keysym.sym == SDLK_TAB) {
				m_controlPanel.m_useMainCam = !m_controlPanel.m_useMainCam;
			}
			else
				_renderer.onKeyPressed(e.key.keysym.sym);
			break;

		case SDL_KEYUP:
			if (e.key.keysym.sym == SDLK_v) {
				m_controlPanel.m_controlMainCam = false;
			}
			break;

		case SDL_MOUSEMOTION:
			//Adjust camera only if left mouse button is pressed
			if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
			{
				camera->adjustOrientation(static_cast<float>(e.motion.xrel), static_cast<float>(e.motion.yrel));
			}
			break;

		default:
			break;
		}
	}
	return true;
}

//=================================================================================
bool Application::PorcessControlsCamPath(std::shared_ptr<I_Camera> camera)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type)
		{
		case SDL_QUIT:
			return false;
			break;

		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE)
				return false;
		}
	}
	ApplyCameraPath(camera);
	return true;
}

//=================================================================================
void Application::ApplyCameraPath(std::shared_ptr<I_Camera> camera)
{
	static double actualTime = 0.0;
	static unsigned int frames = 0;
	const static double waitTime = 1000.0;
	actualTime += m_CameraPathTimer.getElapsedTimeFromLastQueryMilliseconds();
	const double fromPathStarted = actualTime - waitTime;

	// wait for 1 second
	const double totalTime = m_camPath->GetTotalTime();
	if (fromPathStarted > 0.0 && fromPathStarted < totalTime) {

		_renderer.EnableStatistics();
		++frames;
		const double normalizedTime = (actualTime - 1000.0) / totalTime;
		auto key = m_camPath->getKeypoint(normalizedTime);
		auto camera = GetCamManager()->GetMainCamera();
		camera->positionCamera(key);
	}
	if (!m_bPathFinished && fromPathStarted > totalTime) {
		std::cout << "Fps during path: " << static_cast<double>(frames) / (totalTime / 1000.0f) << std::endl;
		CameraPathFinished();
	}
}

//=================================================================================
void Application::CameraPathFinished()
{
	m_bPathFinished = true;
	_renderer.EnableStatistics(false);
	m_statisticsFile.close();
}

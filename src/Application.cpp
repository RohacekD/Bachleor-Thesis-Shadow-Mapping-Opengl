#include "Application.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl_gl3.h>
#include "Debug.h"
#include "DebugCallbacks.h"

//=================================================================================
Application::Application()
{
    _window = nullptr;
}

//=================================================================================
bool Application::Init()
{
    if (!initWindow())
        return false;

    if (!initGlew())
        return false;

	glDebugMessageCallback(debugFunc, nullptr);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplSdlGL3_Init(_window);

	ImGui::StyleColorsDark();

	m_cameraManager = std::make_shared<C_CameraManager>();
	_scene = std::make_shared<Scene>();

	m_controlPanel.m_active = true;
	m_controlPanel.m_controlMainCam = false;
	m_controlPanel.m_controlScene = true;
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

    _window = SDL_CreateWindow("Shadows", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
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
	float r = glm::length(_scene->bbox.maxPoint - _scene->bbox.minPoint) / 2;
	float fovyDeg = 90;
	float fovyRad = glm::radians(fovyDeg);
	float aspectRatio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
	float nearZ = 0.1f;

	float fovXrad = 2 * glm::asin(aspectRatio * glm::sin(fovyRad / 2.0f));
	float d = r / glm::max(sin(fovyRad / 2), sin(fovXrad / 2));

	freeCamera->setupCameraProjection(0.1f, 2 * (d + r), aspectRatio, fovyDeg);
	freeCamera->positionCamera(glm::vec3(0, 0, -1 * (d)), glm::vec3(0));
}

//=================================================================================
bool Application::addModelFileToScene(const char* fileToLoad, std::shared_ptr<Scene> scene, const glm::mat4& transform)
{
    std::unique_ptr<SceneLoader> sl = std::unique_ptr<SceneLoader>(new SceneLoader);

    std::string pth(fileToLoad), directory, filename;
	_splitPathToFilenameAndDirectory(pth, directory, filename);

    if(!sl->addModelFromFileToScene(directory.c_str(), filename.c_str(), _scene, transform))
    {
         Clear();
         return false;
    }

    return true;
}

//=================================================================================
void Application::ShowGUI()
{
	// Create a window called "My First Tool", with a menu bar.
	ImGui::Begin("Controls", &m_controlPanel.m_active);
	ImGui::Checkbox("Disable Controls", &m_controlPanel.m_controlScene);
	ImGui::Checkbox("Use main camera", &m_controlPanel.m_useMainCam);
	ImGui::Checkbox("Control main camera", &m_controlPanel.m_controlMainCam);
	ImGui::End();

}

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
bool Application::Run()
{
    if(!Init())
        return false;

    //Enter Your model path HERE
	if (!addModelFileToScene("models/scene.obj", _scene, glm::translate(glm::vec3(0.0f, 0.0f, 0.0f))))
	{
		std::cerr << "Failed to load scene: " << std::string("models/terrain/terrain.obj") << std::endl;
		return false;
	}

	GetCamManager();

	m_camera = std::make_shared<T_Camera>();
	auto debugCam = std::make_shared<T_Camera>();

	GetCamManager()->SetMainCamera(m_camera);
	GetCamManager()->SetDebugCamera(debugCam);


	setupCamera(m_camera);
	setupCamera(debugCam);

	m_camera->SetFov((60));
	m_camera->SetFar(m_camera->GetFar() / 3);
	m_camera->update();
	debugCam->update();

    //Prepare rendering data
    if(!_renderer.init(_scene, SCREEN_WIDTH, SCREEN_HEIGHT))
         return false;

    _timer.reset();
    bool quit = false;
    SDL_Event e;

    while (!quit)
	{

		auto renderCamera = GetCamManager()->GetActiveCamera();
		auto controledCamera = renderCamera;
		if (m_controlPanel.m_controlMainCam) {
			controledCamera = GetCamManager()->GetMainCamera();
		}
		
        while (SDL_PollEvent(&e) != 0)
		{
			ImGui_ImplSdlGL3_ProcessEvent(&e);
			if(m_controlPanel.m_controlScene) continue;
			controledCamera->Input(e);
            switch (e.type)
            {
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
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
                    controledCamera->adjustOrientation(static_cast<float>(e.motion.xrel), static_cast<float>(e.motion.yrel));
                }
                break;

            default:
                break;
            }
		}
		ImGui_ImplSdlGL3_NewFrame(_window);
		GetCamManager()->UseDebugCam(!m_controlPanel.m_useMainCam);
		ErrorCheck();

		renderCamera->update();
		m_camera->update();
        _renderer.onUpdate(float(_timer.getElapsedTimeFromLastQueryMilliseconds()));

        _renderer.onWindowRedraw(*(renderCamera.get()), renderCamera->getPosition());
		ShowGUI();
		{
			glUseProgram(0);//just to be sure
			RenderDoc::C_DebugScope a("imGUI-render");
			ImGui::Render();
			ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
			ErrorCheck();
		}
        SDL_GL_SwapWindow(_window);
    }

	ImGui_ImplSdlGL3_Shutdown();
    Clear();

    return true;
}

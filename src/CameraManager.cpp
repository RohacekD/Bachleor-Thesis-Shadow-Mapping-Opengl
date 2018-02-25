#include "CameraManager.h"
#include "Debug.h"


//=================================================================================
C_CameraManager::C_CameraManager()
	: m_debugCamera(nullptr)
	, m_mainCamera(nullptr)
{
}

//=================================================================================
C_CameraManager::~C_CameraManager()
{
	//DestructorFullCheck(); Don't call it here because singleton is destroyed on the end of program execution
}

//=================================================================================
std::shared_ptr<I_Camera> C_CameraManager::GetActiveCamera() const
{
	if (m_useDebugCam)
		return m_debugCamera;
	return m_mainCamera;
}

//=================================================================================
std::shared_ptr<I_Camera> C_CameraManager::GetMainCamera() const { 
	return m_mainCamera;
}

//=================================================================================
void C_CameraManager::SetMainCamera(std::shared_ptr<I_Camera> camera) { m_mainCamera = camera; }

//=================================================================================
void C_CameraManager::SetDebugCamera(std::shared_ptr<I_Camera> camera) { m_debugCamera = camera; }

//=================================================================================
void C_CameraManager::DebugDraw() const
{
	m_debugCamera->debugDraw();
	m_mainCamera->debugDraw();
}

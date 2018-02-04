#include "CameraManager.h"


//=================================================================================
C_CameraManager::C_CameraManager()
	: m_debugCamera(nullptr)
	, m_activeCamera(nullptr)
{
}

//=================================================================================
std::shared_ptr<I_Camera> C_CameraManager::GetViewCamera() const
{
	if (m_useDebugCam)
		return m_debugCamera;
	return m_activeCamera;
}

//=================================================================================
std::shared_ptr<I_Camera> C_CameraManager::GetActiveCamera() const { 
	return m_activeCamera;
}

void C_CameraManager::SetActiveCamera(std::shared_ptr<I_Camera> camera) { m_activeCamera = camera; }

void C_CameraManager::SetDebugCamera(std::shared_ptr<I_Camera> camera) { m_debugCamera = camera; }

void C_CameraManager::DebugDraw() const
{
	m_debugCamera->debugDraw();
	m_activeCamera->debugDraw();
}

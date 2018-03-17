/**
 * ==============================================
 * @file 		CameraManager.h
 * @date 		2018/03/17 19:21
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology
 * @university 	Brno University of Technology
 *
 * @author	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
 */
 
#pragma once

#include "Camera/ICamera.h"

#include <memory>

/**
 * ==============================================
 * @class C_CameraManager
 *
 * @brief Holds all cameras on the scene and switch between them
 *
 * I created this system to allow observe scene with different camera 
 * than one bound to the CSM. Because it is hard to debug graphical 
 * glitches on the borders between split frustums. For now exists 
 * two cameras in the scene.
 * Main camera	- this camera is one to which is CSM bound
 * Debug camera - this camera is here just for debug purposes
 * 
 * @author	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date	2018/03/17
 * ==============================================
 */
class C_CameraManager {
public:
	C_CameraManager();
	~C_CameraManager();

	/**
	 * ==============================================
	 * @method:    GetActiveCamera
	 * @fullName:  C_CameraManager::GetActiveCamera
	 * @return:    std::shared_ptr<I_Camera>
	 * @brief	   Returns currently used camera. Depends on @see m_useDebugCam
	 * ==============================================
	 */
	std::shared_ptr<I_Camera> GetActiveCamera() const;

	std::shared_ptr<I_Camera> GetMainCamera() const;
	void SetMainCamera(std::shared_ptr<I_Camera> camera);

	std::shared_ptr<I_Camera> GetDebugCamera() const { return m_debugCamera; }
	void SetDebugCamera(std::shared_ptr<I_Camera> camera);

	void UseDebugCam(bool use = true) noexcept { m_useDebugCam = use; }
	bool IsUsingDebugCam() const noexcept { return m_useDebugCam; }

	void DebugDraw() const;

private:
	bool m_useDebugCam;

	std::shared_ptr<I_Camera> m_mainCamera;
	std::shared_ptr<I_Camera> m_debugCamera; 
};
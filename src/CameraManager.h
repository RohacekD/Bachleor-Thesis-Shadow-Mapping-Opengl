#pragma once

#include <memory>
#include "Camera/ICamera.h"

class C_CameraManager {
public:
	C_CameraManager();

	std::shared_ptr<I_Camera> GetViewCamera() const;

	std::shared_ptr<I_Camera> GetActiveCamera() const;
	void SetActiveCamera(std::shared_ptr<I_Camera> camera);

	std::shared_ptr<I_Camera> GetDebugCamera() const { return m_debugCamera; }
	void SetDebugCamera(std::shared_ptr<I_Camera> camera);

	void UseDebugCam(bool use = true) { m_useDebugCam = use; }
	bool IsUsingDebugCam() const { return m_useDebugCam; }

	void DebugDraw() const;

private:
	bool m_useDebugCam;

	std::shared_ptr<I_Camera> m_activeCamera;
	std::shared_ptr<I_Camera> m_debugCamera;
};
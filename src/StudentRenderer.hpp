#pragma once

#include "IRenderer.h"

#include <SDL/SDL.h>
#include <GL/glew.h>

#include "Scene.hpp"
#include "ShaderCompiler.hpp"
#include "HighResolutionTimer.hpp"
#include "CameraPath.h"
#include "ShadowMapCascade.h"

class I_Camera;

namespace GLW {
	class C_Texture;
	class C_Framebuffer;
}

namespace render {
	class C_Scene;
}


namespace Animation {
	class C_Animation;
}

class StudentRenderer : public I_Renderer
{
public:
	StudentRenderer();
	~StudentRenderer();;

	bool init(std::shared_ptr<Scene> scene, unsigned int screenWidth, unsigned int screenHeight) override;
	void onUpdate(float timeSinceLastUpdateMs) override;
	void onWindowRedraw(const I_Camera& camera, const  glm::vec3& cameraPosition) override;
	void onKeyPressed(SDL_Keycode code) override;
	void clearStudentData() override;

private:
	void renderToFBO(const glm::mat4& cameraViewProjectionMatrix) const;
	void renderDepthSamples() const;
	bool initFBO();
	void ShowGUI();
	struct {
		bool m_active;
		float m_lambda;
		int m_splits;
	} m_ControlPanel;

	glm::mat4 GetShadowViewMat() const;
	glm::mat4 GetShadowProjectionMat() const;
	glm::mat4 ScreenToTextureCoord() const;

	std::shared_ptr<Scene> m_scene;
	std::shared_ptr<render::C_Scene> m_renderScene;

	static const int gs_splits;

	std::shared_ptr<GLW::C_Framebuffer> m_framebuffer;
	std::shared_ptr<GLW::C_Framebuffer> m_DepthSamplesframebuffer;

	bool m_bWireframe;
	unsigned int m_screenWidht;
	unsigned int m_screenHeight;

	// not really accurate rolling average but I am ok with it
	double approxRollingAverage(double new_sample);
	void printFrameStatistics() const;

	double m_avg;
	unsigned long m_frameID;
	const static long s_avgFramesNum = 10000;

	std::shared_ptr<C_ShadowMapCascade> m_CSM;


	HighResolutionTimer  m_timer;
	std::shared_ptr<Animation::C_Animation> m_animation;
};

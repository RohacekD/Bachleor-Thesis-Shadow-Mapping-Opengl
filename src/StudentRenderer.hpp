/** ==============================================
 * @file 		StudentRenderer.hpp
 * @date 		2018/03/17 20:02
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Ing. Jozef Kobrtek
 * Contact: 	ikobrtek@fit.vutbr.cz
 * @edited 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
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
class C_SDSMSplitsCalculator;

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
		bool m_useSDSM;
		bool m_animateSun;
	} m_ControlPanel;

	std::shared_ptr<Scene> m_scene; // IMHO I don't need it anymore
	std::shared_ptr<render::C_Scene> m_renderScene;

	static const int gs_splits;

	std::shared_ptr<C_SDSMSplitsCalculator> m_sdsmCalc;
	std::shared_ptr<GLW::C_Framebuffer> m_framebuffer;
	std::shared_ptr<GLW::C_Framebuffer> m_DepthSamplesframebuffer;

	unsigned int m_screenWidht;
	unsigned int m_screenHeight;

	// not really accurate rolling average but I am ok with it - not anymore
	double approxRollingAverage(double new_sample);
	void printFrameStatistics() const;

	double m_avg;
	unsigned long m_frameID;
	const static long s_avgFramesNum = 10000;

	std::shared_ptr<C_ShadowMapCascade> m_CSM;

	std::shared_ptr<Animation::C_Animation> m_SunAnimation;
};

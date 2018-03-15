#include "StudentRenderer.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "animation/Animation.h"
#include "animation/TranslationAnimations.h"
#include "HighResolutionTimer.hpp"
#include "render/Nodes/Scene.h"
#include "render/Nodes/MeshNode.h"
#include "render/SceneBuilder.h"
#include "render/RenderParams.h"

#include "GLW/Texture.h"
#include "GLW/Framebuffer.h"

#include "ShaderManager.h"
#include "Camera/ICamera.h"
#include "CameraManager.h"
#include "Application.hpp"
#include "UniformBuffersManager.h"

#include "imgui/imgui.h"

#include <glm/gtx/string_cast.hpp>

#include "Debug.h"

#include <iostream>
#include <iterator>

//if I want to do this properly I need also make changes to the shaders
//#define FBO_COLOR

const int gs_shadowMapsize = 2048;
const int StudentRenderer::gs_splits = 4;

//=================================================================================
// StudentRenderer
//=================================================================================
StudentRenderer::StudentRenderer()
	:m_bWireframe(false)
{

}

//=================================================================================
StudentRenderer::~StudentRenderer() {
	printFrameStatistics();
}

//=================================================================================
bool StudentRenderer::init(std::shared_ptr<Scene> scene, unsigned int screenWidth, unsigned int screenHeight)
{
	HighResolutionTimer timer;
	timer.reset();
	m_scene = scene;
	m_screenWidht = screenWidth;
	m_screenHeight = screenHeight;
	render::C_SceneBuilder sceneBuilder;
	m_renderScene = sceneBuilder.LoadScene("models/scene.xml");
	m_renderScene->m_bbox = scene->bbox;
	m_avg = 0.0f;
	m_frameID = 0;

	// I should pass camera just to CSM
	auto camera = Application::Instance().GetCamManager()->GetMainCamera();
	auto lightInfo = std::make_shared<C_DirectionalLight>(camera, glm::vec3(0.0f, 3.0f, 0.0f) * 15.0f, glm::vec3(0.0, 0.0, 0.0), 1.0f);
	m_animation = std::make_shared<Animation::C_Animation>(5000.0f);
	m_animation->AddComponent(std::make_shared<Animation::C_ElipseTranslateAnim>(10.0f, 10.0f));
	lightInfo->SetDirectionAnimation(m_animation);

	m_CSM = std::make_shared<C_ShadowMapCascade>(lightInfo, camera, gs_shadowMapsize, gs_splits);

	if (!initFBO()) {
		return false;
	}
	glEnable(GL_CULL_FACE);
	glClearColor(static_cast<GLclampf>(.26), static_cast<GLclampf>(.26), static_cast<GLclampf>(.26), static_cast<GLclampf>(1.0));

	std::cout << C_ShaderManager::Instance().ShadersStatistics();

	std::cout << "Loading time: " << timer.getElapsedTimeFromLastQueryMilliseconds() << "ms" << std::endl;

	return true;
}

//=================================================================================
void StudentRenderer::onUpdate(float timeSinceLastUpdateMs)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	approxRollingAverage(timeSinceLastUpdateMs);
	m_renderScene->Update(timeSinceLastUpdateMs);
	++m_frameID;
	std::cout << timeSinceLastUpdateMs << std::endl;
	m_animation->Update(timeSinceLastUpdateMs);
	m_CSM->Update();
}

//=================================================================================
void StudentRenderer::onKeyPressed(SDL_Keycode code)
{
	switch (code) {
	case SDLK_w:
	{
		m_bWireframe = !m_bWireframe;
		break;
	}
	case SDLK_i:
	{
		printFrameStatistics();
		break;
	}
	case  SDLK_UP:
	{
		m_CSM->ChangeLambda(0.1f);
		std::cout << m_CSM->GetLambda() << std::endl;
	}
	break;
	case  SDLK_DOWN:
	{
		m_CSM->ChangeLambda(-0.1f);
		std::cout << m_CSM->GetLambda() << std::endl;
	}
	break;
	}
}

//=================================================================================
void StudentRenderer::onWindowRedraw(const I_Camera& camera, const  glm::vec3& cameraPosition)
{
	ShowGUI();
	renderToFBO(camera.getViewProjectionMatrix());


	RenderDoc::C_DebugScope scope("Render pass");

	m_CSM->ActivateUBO();

	glViewport(0, 0, m_screenWidht, m_screenHeight);

	render::S_RenderParams params;
	params.m_cameraViewProjectionMatrix = camera.getViewProjectionMatrix();
	params.m_cameraPosition = m_CSM->GetBoundCamera()->getPosition();
	params.m_shadowMap = m_framebuffer->GetAttachement(GL_DEPTH_ATTACHMENT);
	params.m_pass = render::S_RenderParams::E_PassType::E_P_RenderPass;

	m_renderScene->Render(params);
	ErrorCheck();
	m_CSM->ActivateUBO(false);

	{
		RenderDoc::C_DebugScope scope("DebugDraw");
		m_CSM->DebugDrawAABBs(camera.getViewProjectionMatrix());
		Application::Instance().GetCamManager()->DebugDraw();
	}
}

//=================================================================================
void StudentRenderer::clearStudentData()
{
	m_framebuffer.reset();
	m_scene.reset();
	m_CSM.reset();
	m_renderScene.reset();
	C_ShaderManager::Instance().Clear();
	C_DebugDraw::Instance().Clear();
	C_UniformBuffersManager::Instance().Clear();
	DestructorFullCheck(); // I have to call it here because destructor is called after OpenGL context is destroyed

	std::cout << "Clear ended after this NO OpenGL should be call" << std::endl;
}

//=================================================================================
void StudentRenderer::renderToFBO(const glm::mat4& cameraViewProjectionMatrix) const
{
	RenderDoc::C_DebugScope scope("Shadow pass");

	m_framebuffer->Bind();
	m_CSM->ActivateUBO();
#ifdef FBO_COLOR
	glDrawBuffer(GL_NONE);
#endif


	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, gs_shadowMapsize, gs_shadowMapsize);

	render::S_RenderParams params;
	params.m_pass = render::S_RenderParams::E_PassType::E_P_ShadowPass;

	glCullFace(GL_FRONT);
	m_renderScene->Render(params);
	glCullFace(GL_BACK);
	ErrorCheck();
	

	m_framebuffer->Unbind();
	m_CSM->ActivateUBO(false);
#ifdef FBO_COLOR
	glDrawBuffer(GL_BACK);
#endif
}

//=================================================================================
bool StudentRenderer::initFBO()
{
	m_framebuffer = std::make_shared<GLW::C_Framebuffer>();

	auto depthTexture = std::make_shared<GLW::C_Texture>("depthTexture", GL_TEXTURE_2D_ARRAY);
	depthTexture->StartGroupOp();
	glTexStorage3D(depthTexture->GetTarget(), 1, GL_DEPTH_COMPONENT32, gs_shadowMapsize, gs_shadowMapsize, m_CSM->GetNumLevels());

	ErrorCheck();
	depthTexture->setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	depthTexture->setFilter(GL_NEAREST, GL_NEAREST);
	glm::vec4 borderColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(borderColor));
	ErrorCheck();

	depthTexture->EndGroupOp();

	m_framebuffer->AttachTexture(GL_DEPTH_ATTACHMENT, depthTexture);

#ifdef FBO_COLOR
	auto colorTexture = std::make_shared<GLW::C_Texture>("colorTexture", GL_TEXTURE_2D_ARRAY);
	colorTexture->StartGroupOp();
	glTexStorage3D(colorTexture->GetTarget(), 1, GL_RGBA32F, gs_shadowMapsize, gs_shadowMapsize, m_CSM->GetNumLevels());

	ErrorCheck();
	colorTexture->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	colorTexture->setFilter(GL_NEAREST, GL_NEAREST);
	ErrorCheck();

	colorTexture->EndGroupOp();

	m_framebuffer->AttachTexture(GL_COLOR_ATTACHMENT0, colorTexture);
#endif
	return true;
}

//=================================================================================
void StudentRenderer::ShowGUI()
{
	ImGui::Begin("Settings", &m_ControlPanel.m_active);
	ImGui::SliderFloat("Lambda", &m_ControlPanel.m_lambda, 0.0f, 1.0f);
	//ImGui::SliderInt("Splits", &m_ControlPanel.m_lambda, 1, 6);
	ImGui::End();

	m_CSM->SetLambda(m_ControlPanel.m_lambda);
}

//=================================================================================
glm::mat4 StudentRenderer::GetShadowViewMat() const
{
	float maxY = m_renderScene->m_bbox.maxPoint.y;
	return glm::lookAt(glm::vec3(0, maxY + 0.5f, 0.0f), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
}

//=================================================================================
glm::mat4 StudentRenderer::GetShadowProjectionMat() const
{
	AABB box = m_renderScene->m_bbox.getTransformedAABB(GetShadowViewMat());

	return glm::ortho(box.minPoint.x, box.maxPoint.x, box.minPoint.y, box.maxPoint.y, 0.5f, -box.minPoint.z);

}

//=================================================================================
glm::mat4 StudentRenderer::ScreenToTextureCoord() const
{
	glm::mat4 offset(1.0f);
	offset = glm::translate(offset, glm::vec3(0.5f, 0.5f, 0.5f));
	offset = glm::scale(offset, glm::vec3(0.5f, 0.5f, 0.5f));
	return offset;
}

//=================================================================================
inline double StudentRenderer::approxRollingAverage(double new_sample) {
	m_avg -= m_avg / s_avgFramesNum;
	m_avg += new_sample / s_avgFramesNum;

	return m_avg;
}

//=================================================================================
void StudentRenderer::printFrameStatistics() const
{
	using std::cout;
	using std::endl;
	cout << "Frame: " << m_frameID << endl;
	cout << "Approx avg frame time: " << m_avg << "ms" << endl;
	if (s_avgFramesNum > m_frameID) {
		cout << "Statistic irrelevant because too few frames passed" << endl;
	}
}

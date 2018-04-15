#include "StudentRenderer.hpp"

#include "animation/Animation.h"
#include "animation/TranslationAnimations.h"
#include "HighResolutionTimer.hpp"
#include "render/Nodes/Scene.h"
#include "render/Nodes/MeshNode.h"
#include "render/SceneBuilder.h"
#include "render/RenderParams.h"

#include "GLW/Texture.h"
#include "GLW/Framebuffer.h"

#include "GLW/Buffers/ShaderStorage.h"
#include "GLW/ShaderProgram.h"
#include "ShaderManager.h"
#include "Camera/ICamera.h"
#include "CameraManager.h"
#include "Application.hpp"
#include "UniformBuffersManager.h"
#include "FrameConstantsBuffer.h"

#ifndef SPEEDPROFILE
#include "imgui/imgui.h"
#endif

#include "SDSMSplitsCalculator.h"
#include "PSSMSplitsCalculator.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Debug.h"

#include "SDSMSplitsCalculator.h"

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
{
	m_ControlPanel.m_animateSun = true;
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
	m_renderScene = sceneBuilder.LoadScene("models/DesertCity.xml");
	m_renderScene->m_bbox = scene->bbox;
	m_avg = 0.0f;
	m_frameID = 0;

	// I should pass camera just to CSM
	auto camera = Application::Instance().GetCamManager()->GetMainCamera();
	auto lightInfo = std::make_shared<C_DirectionalLight>(camera, glm::vec3(0.0f, 3.0f, 0.0f) * 15.0f, glm::vec3(0.0, 0.0, 0.0), 1.0f);

	// create animation for sun
	m_SunAnimation = std::make_shared<Animation::C_Animation>(5000.0f);
	m_SunAnimation->AddComponent(std::make_shared<Animation::C_ElipseTranslateAnim>(10.0f, 10.0f));
	lightInfo->SetDirectionAnimation(m_SunAnimation);

	m_CSM = std::make_shared<C_ShadowMapCascade>(lightInfo, camera, gs_shadowMapsize, gs_splits, std::make_shared<C_PSSMSplitsCalculator>(gs_splits, camera));

	UseSDSM(m_ControlPanel.m_useSDSM);

	if (!initFBO()) {
		return false;
	}
	glEnable(GL_CULL_FACE);
	glClearColor(static_cast<GLclampf>(.26), static_cast<GLclampf>(.26), static_cast<GLclampf>(.26), static_cast<GLclampf>(1.0));


	m_FrameConstUBO = C_UniformBuffersManager::Instance().CreateUniformBuffer<C_FrameConstantsBuffer>("frameConst");

	std::cout << C_ShaderManager::Instance().ShadersStatistics();

	std::cout << "Loading time: " << timer.getElapsedTimeFromLastQueryMilliseconds() << "ms" << std::endl;

	return true;
}

//=================================================================================
void StudentRenderer::onUpdate(float timeSinceLastUpdateMs)
{
	C_ShaderManager::Instance().Update();
	approxRollingAverage(timeSinceLastUpdateMs);
	m_renderScene->Update(timeSinceLastUpdateMs);
	++m_frameID;
	if (m_ControlPanel.m_animateSun) {
		m_SunAnimation->Update(timeSinceLastUpdateMs);
	}
	UseSDSM(m_ControlPanel.m_useSDSM);
	m_CSM->Update();
}

//=================================================================================
void StudentRenderer::onKeyPressed(SDL_Keycode code)
{
	switch (code) {
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
#ifndef SPEEDPROFILE
	ShowGUI();
#endif
	// if use SDSM - but better use depth from last render pass
	auto method = m_CSM->GetSplittingMethod(); // move it here until C++17 is used
	if (method->MethodType() == I_SplitPlanesCalculator::E_MethodType::SDSM) {
		renderDepthSamples();
		auto calc = std::dynamic_pointer_cast<C_SDSMSplitsCalculator>(method);
		calc->RecalcSplits(m_DepthSamplesframebuffer->GetAttachement(GL_DEPTH_ATTACHMENT));
	}

	renderToFBO(camera.getViewProjectionMatrix());


	RenderDoc::C_DebugScope scope("Render pass");

	m_CSM->ActivateUBO();

	glViewport(0, 0, m_screenWidht, m_screenHeight);

	m_FrameConstUBO->SetViewProjection(camera.getViewProjectionMatrix());
	m_FrameConstUBO->SetCameraPosition(glm::vec4(m_CSM->GetBoundCamera()->getPosition(), 1.0f));
	m_FrameConstUBO->UploadData();
	m_FrameConstUBO->Activate(true);

	render::S_RenderParams params;
	params.m_cameraPosition = m_CSM->GetBoundCamera()->getPosition();
	params.m_shadowMap = m_framebuffer->GetAttachement(GL_DEPTH_ATTACHMENT);
	params.m_pass = render::S_RenderParams::E_PassType::RenderPass;
	params.m_FrustumSphere = m_CSM->GetBoundCamera()->getFrustum().GetAABB().GetSphere();

	glActiveTexture(GL_TEXTURE0);
	params.m_shadowMap->bind();

	m_renderScene->Render(params, glm::mat4(1.0f));

	params.m_shadowMap->unbind();

	ErrorCheck();
	m_FrameConstUBO->Activate(false);
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
	m_DepthSamplesframebuffer.reset();
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
void StudentRenderer::UseSDSM(bool sdsm /*= true*/)
{
	if (sdsm) {
		m_CSM->SetSplittingMethod(I_SplitPlanesCalculator::E_MethodType::SDSM);
	}
	else {
		m_CSM->SetSplittingMethod(I_SplitPlanesCalculator::E_MethodType::PSSM);
	}
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
	params.m_pass = render::S_RenderParams::E_PassType::ShadowPass;

	//glCullFace(GL_FRONT);
	m_renderScene->RenderChilds(params, glm::mat4(1.0f));
	//glCullFace(GL_BACK);
	ErrorCheck();
	

	m_framebuffer->Unbind();
	m_CSM->ActivateUBO(false);
#ifdef FBO_COLOR
	glDrawBuffer(GL_BACK);
#endif
}

//=================================================================================
void StudentRenderer::renderDepthSamples() const
{
	RenderDoc::C_DebugScope scope("Z Pass");
	auto mainCam = Application::Instance().GetCamManager()->GetMainCamera();

	m_DepthSamplesframebuffer->Bind();

	m_FrameConstUBO->SetViewProjection(mainCam->getViewProjectionMatrix());
	m_FrameConstUBO->UploadData();
	m_FrameConstUBO->Activate(true);


	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 512, 512);

	render::S_RenderParams params;
	params.m_pass = render::S_RenderParams::E_PassType::ZPass;

	m_renderScene->RenderChilds(params, glm::mat4(1.0f));
	ErrorCheck();

	m_FrameConstUBO->Activate(false);
	m_DepthSamplesframebuffer->Unbind();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

//=================================================================================
bool StudentRenderer::initFBO()
{
	m_framebuffer = std::make_shared<GLW::C_Framebuffer>();

	auto depthTexture = std::make_shared<GLW::C_Texture>("depthTexture", GL_TEXTURE_2D_ARRAY);
	depthTexture->StartGroupOp();
	glTexStorage3D(depthTexture->GetTarget(), 1, GL_DEPTH_COMPONENT32, gs_shadowMapsize, gs_shadowMapsize, m_CSM->GetNumLevels());
	depthTexture->SetDimensions({gs_shadowMapsize, gs_shadowMapsize });

	ErrorCheck();
	depthTexture->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	depthTexture->SetFilter(GL_NEAREST, GL_NEAREST);
	depthTexture->SetTexParameter(GL_TEXTURE_BORDER_COLOR, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	depthTexture->EndGroupOp();

	m_framebuffer->AttachTexture(GL_DEPTH_ATTACHMENT, depthTexture);

#ifdef FBO_COLOR
	auto colorTexture = std::make_shared<GLW::C_Texture>("colorTexture", GL_TEXTURE_2D_ARRAY);
	colorTexture->StartGroupOp();
	glTexStorage3D(colorTexture->GetTarget(), 1, GL_RGBA32F, gs_shadowMapsize, gs_shadowMapsize, m_CSM->GetNumLevels());
	colorTexture->SetDimensions({ gs_shadowMapsize, gs_shadowMapsize });

	ErrorCheck();
	colorTexture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	colorTexture->SetFilter(GL_NEAREST, GL_NEAREST);
	ErrorCheck();

	colorTexture->EndGroupOp();

	m_framebuffer->AttachTexture(GL_COLOR_ATTACHMENT0, colorTexture);
#endif

	m_DepthSamplesframebuffer = std::make_shared<GLW::C_Framebuffer>();

	auto depthSamplesTexture = std::make_shared<GLW::C_Texture>("depthSamplesTexture");
	depthSamplesTexture->StartGroupOp();
	glTexImage2D(depthSamplesTexture->GetTarget(),
		0,
		GL_DEPTH_COMPONENT32,
		512,
		512,
		0,
		GL_DEPTH_COMPONENT,
		GL_UNSIGNED_BYTE,
		0);
	depthSamplesTexture->SetDimensions({ 512, 512 });

	ErrorCheck();
	depthSamplesTexture->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	depthSamplesTexture->SetFilter(GL_NEAREST, GL_NEAREST);
	ErrorCheck();

	depthSamplesTexture->EndGroupOp();

	m_DepthSamplesframebuffer->AttachTexture(GL_DEPTH_ATTACHMENT, depthSamplesTexture);


	return true;
}

#ifndef SPEEDPROFILE
//=================================================================================
void StudentRenderer::ShowGUI()
{
	m_ControlPanel.m_lambda = m_CSM->GetLambda();
	m_ControlPanel.m_useSDSM = (m_CSM->GetSplittingMethodType() == I_SplitPlanesCalculator::E_MethodType::SDSM);

	bool active = true;
	if (m_ControlPanel.m_useSDSM) {
		ImGui::Begin("Depth histogram", &active);
#ifdef _DEBUG
		ImGui::Image((void*)std::dynamic_pointer_cast<C_SDSMSplitsCalculator>(m_CSM->GetSplittingMethod())->GetHistogramTexture()->GetTexture(),
		{ 256, 150 }, { 0,1 }, { 1,0 });
#endif // _DEBUG
		ImGui::End();
	}


	ImGui::Begin("Settings", &m_ControlPanel.m_active);
	ImGui::SliderFloat("Lambda", &m_ControlPanel.m_lambda, 0.0f, 1.0f);
	ImGui::Checkbox("Use SDSM", &m_ControlPanel.m_useSDSM);
	ImGui::Checkbox("Animate sun position", &m_ControlPanel.m_animateSun);
	ImGui::End();

	m_CSM->SetLambda(m_ControlPanel.m_lambda);
}
#endif

//=================================================================================
inline double StudentRenderer::approxRollingAverage(double new_sample) {
	static const float smoothing = 0.9f; // larger=more smoothing
	m_avg = (m_avg * smoothing) + (new_sample * (1.0f - smoothing));
	//m_avg -= m_avg / s_avgFramesNum;
	//m_avg += new_sample / s_avgFramesNum;

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
